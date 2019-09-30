#include "StarNetInstance.h"
#include "StarNetParameters.h"
#include "tf_utils.h"

#include <pcl/AutoViewLock.h>
#include <pcl/Console.h>
#include <pcl/StdStatus.h>
#include <pcl/View.h>
#include <numeric>

namespace pcl {
    StarNetInstance::StarNetInstance(const MetaProcess* m):
       ProcessImplementation(m),
       p_mask(TheMaskParameter->DefaultValue()),
       p_stride(StarNetStride::Default)
    {}

    StarNetInstance::StarNetInstance(const StarNetInstance& x):
       ProcessImplementation(x)
    {
       Assign(x);
    }

    void StarNetInstance::Assign(const ProcessImplementation& p) {
        const StarNetInstance* x = dynamic_cast<const StarNetInstance*>(&p);
        if (x != nullptr)
        {
            p_mask      = x->p_mask;
            p_stride    = x->p_stride;
        }
    }

    bool StarNetInstance::CanExecuteOn(const View& view, String& whyNot) const {
        if (view.Image().IsComplexSample()) {
            whyNot = "StarNet cannot be executed on complex images.";
            return false;
        }
        return true;
    }
    // TO DO:
	// 1. Somehow specify where exactly PI should look for model weights.
	// 2. Make so that if user selects 'Create starmask' the mask appears in a new window.
	//    Could not figure out how to do that.
	// 3. Make so that PI does not freeze while the script is running.
    class StarNetEngine {
    public:
        template <class P>
        static void Apply(GenericImage<P>& image, View& view, const StarNetInstance& instance) {
            int window = 256;  // input tile size
            int finalHeight;   // final height after all augmentations
            int finalWidth;    // final width after all augmentations
            int size;          // total numer of pixels in each channel
            int border;        // border of the image due to stride being less that window
            
            Console().WriteLn("<end><cbr>Starting star removal procedure...");
            
			// getting some info about our image
            int32 imgNumChannels = image.NumberOfChannels();
            int32 imgHeight = image.Height();
            int32 imgWidth = image.Width();
            int32 imgColorSpace = image.ColorSpace();
            int32 imgBitsPerSample = image.BitsPerSample();
            int32 strideValue = pow(2, 7 - instance.p_stride);
            
            bool  imgHasAlphaChannels = image.HasAlphaChannels();
            bool  imgFloatSample = image.IsFloatSample();
            
			// some simple cheks
            if (imgColorSpace != 0 && imgColorSpace != 1) throw Error("Only RGB and Greyscale images are supported!");
            if (imgNumChannels != 1 && imgNumChannels != 3) throw Error("Weird number of channels: " + String(imgNumChannels) + "! Should be 1 or 3!");
            if (imgHeight < 256 || imgWidth < 256) throw Error("Minimum image size is 256 by 256!");
            
            // just for the sake of debugging
            Console().WriteLn("Stride: " + String(strideValue));
            Console().WriteLn("Image size: " + String(imgHeight) + "x" + String(imgWidth));
            Console().WriteLn("Number of channels: " + String(imgNumChannels));
            Console().WriteLn("Color space: " + ColorSpace::Name(imgColorSpace));
            Console().WriteLn("Bits per sample: " + String(imgBitsPerSample));
            Console().WriteLn("Has alpha channels: " + String(imgHasAlphaChannels));
            Console().WriteLn("Float sample: " + String(imgFloatSample));
            
            
			// starting augmentation of the image
			
			// because we process image tile by tile of size 256 by 256 and then only use central part of the final output
			// of the size stride by stride, there is a restriction on the size of the image we can process:
			// 
			// size of the image should be a multiple of stride plus some additional margin of the size (256 - stride) / 2
			// to allow to process the very edges of an image
			
			// this proper size is called finalHeight by finalWidth and is calculated below
			
            border = (window - strideValue) / 2;
            
            if (imgHeight % strideValue != 0) {
                finalHeight = (imgHeight / strideValue + 1) * strideValue + 2 * border;
            } else {
                finalHeight = imgHeight + 2 * border;
            }
            if (imgWidth % strideValue != 0) {
                finalWidth = (imgWidth / strideValue + 1) * strideValue + 2 * border;
            } else {
                finalWidth = imgWidth + 2 * border;
            }
            
            size = finalWidth * finalHeight;
            
			// containers to hold augmented images
			
			// original image with added edges
            GenericImage<P> transformed(finalWidth, finalHeight, image.ColorSpace());
			// final image after star removal
            GenericImage<P> starless(finalWidth, finalHeight, image.ColorSpace());
            
			
			// the whole bunch of loops below calculates pixel values for the added edges
			// we fill them with mirrored edges of the original image
			
            int left = border;
            int right = finalWidth - imgWidth - left;
            int top = border;
            int bottom = finalHeight - imgHeight - border;
            
            for (int c = 0; c < imgNumChannels; c++) {
                for (int i = 0; i < imgWidth; i++) {
                    for (int j = 0; j < imgHeight; j++) {
                            transformed(i + left, j + top, c) = image(i, j, c);
                    }
                }
                
                for (int i = 0; i < left; i++) {
                    for (int j = 0; j < imgHeight; j++) {
                        transformed(left - i - 1, j + top, c) = transformed(left + i, j + top, c);
                    }
                }
                
                for (int i = 0; i < right; i++) {
                    for (int j = 0; j < imgHeight; j++) {
                        transformed(finalWidth - right + i, j + top, c) = transformed(finalWidth - right - i - 1, j + top, c);
                    }
                }
                
                for (int i = 0; i < finalWidth; i++) {
                    for (int j = 0; j < top; j++) {
                        transformed(i, top - j - 1, c) = transformed(i, top + j, c);
                    }
                }
                
                for (int i = 0; i < finalWidth; i++) {
                    for (int j = 0; j < bottom; j++) {
                        transformed(i, finalHeight - bottom + j, c) = transformed(i, finalHeight - bottom - j - 1, c);
                    }
                }
                
            }
            
			// finished augmentation of the image
			
			
			// now its time for star removal
            Console().WriteLn("Restoring neural network checkpoint... ");
            TF_Graph* graph;
            
			// depending on the number of channels, we load either model for RGB image
			// or Grayscale images
            if (imgNumChannels == 3) graph = tf_utils::LoadGraphDef("rgb_starnet_weights.pb");
            else if (imgNumChannels == 1) graph = tf_utils::LoadGraphDef("mono_starnet_weights.pb");
            else Console().WriteLn("Weird number of channels: " + String(imgNumChannels));
            
			// this gives problems at the moment - we need to make PI to look for the weights
			// above always in the 'bin' folder or in some other fixed location
            if(!graph) throw Error("Checkpoint file not found!");
            
			
			// the code below is just Tensorflow stuff
			
            TF_Output inputOp = {TF_GraphOperationByName(graph, "X"), 0};
            
			// specification of input dimentions
            const std::vector<std::int64_t> inputDims = {1, window, window, imgNumChannels};
            
			// creating all necessary tensors and session for TF
            TF_Tensor* inputTensor;
            TF_Output outOp;
            TF_Tensor* outputTensor;
            TF_Status* status;
            TF_SessionOptions* options;
            TF_Session* sess;
			
			// vector for input data
            std::vector<float> inputVals;
			
			// pointer to output data
			float* data;
            
            options = TF_NewSessionOptions();
            status = TF_NewStatus();
            outOp = {TF_GraphOperationByName(graph, "generator/g_deconv7/Sub"), 0};
            sess = TF_NewSession(graph, options, status);
            
			
			// number of steps to process the whole image and counter
            float max = ((finalHeight - window) /strideValue + 1) * ((finalWidth - window) / strideValue + 1);
            float counter = 0;
            
            Console().WriteLn("Total number of tiles: " + String(max));
            
			// this is the main loop  - it goes through the whole image tile by tile and processes it
			
            for (int i = 0; i <= finalWidth - window; i += strideValue) {
                for (int j = 0; j <= finalHeight - window; j += strideValue) {
                    
					// filling the input vector for TF
					
					
					// rescaling the values - at the moment RGB model works with samples in the range [-1, 1]
					// and Grayscale model works with samples with zero mean and standard deviation of one
					// this will be changed in the future, I am plannning to make both models works with samples
					// in [-1, 1] range
                    for (int w = i; w < i + window; w++) {
                        for (int h = j; h < j + window; h++) {
                            for (int c = 0; c < imgNumChannels; c++) {
                                if (imgNumChannels == 3) {
                                    if (!imgFloatSample) inputVals.push_back((float) transformed(w, h, c) / pow(2, imgBitsPerSample - 1) - 1);
                                    else inputVals.push_back((float) transformed(w, h, c) * 2 - 1);
                                } else {
                                    if (!imgFloatSample) inputVals.push_back((float) transformed(w, h, c) / pow(2, imgBitsPerSample));
                                    else inputVals.push_back((float) transformed(w, h, c));
                                }
                            }
                        }
                    }
                    
					
					// calculation of standard deviation and mean for Grayscale model
					// I am plannning to make Grayscale model to work with samples rescaled to
					// [-1, 1] instead, so this is likely to be removed in the future
                    double sum = std::accumulate(std::begin(inputVals), std::end(inputVals), 0.0);
                    double mean = sum / inputVals.size();
                    
                    double sq_sum = std::inner_product(inputVals.begin(), inputVals.end(), inputVals.begin(), 0.0);
                    double stdev = std::sqrt(sq_sum / inputVals.size() - mean * mean);
                    
                    if (stdev == 0) stdev = 1;
                    if(imgNumChannels == 1) {
                        for (int i = 0; i < inputVals.size(); i++) {
                            inputVals[i] = (inputVals[i] - mean) / stdev;
                        }
                    }
                    
					
					// load values into tensor
                    inputTensor = tf_utils::CreateTensor(TF_FLOAT,
                                                         inputDims.data(), inputDims.size(),
                                                         inputVals.data(), inputVals.size() * sizeof(float));
                    
                    outputTensor = nullptr;
					
					// run TF to get output
                    TF_SessionRun(sess, nullptr,                 // run options
                                  &inputOp, &inputTensor, 1,     // input tensors, input tensor values, number of inputs
                                  &outOp, &outputTensor, 1,      // output tensors, output tensor values, number of outputs
                                  nullptr, 0, nullptr, status);  // target operations, number of targets, run metadata, output status
                    
					// get the output
                    data = (float*) (TF_TensorData(outputTensor));
                    
                    
					// basically, just primitive loop to load vector of values into pixels
					// and rescale them back
					// plus a trick to prevent overflow
					
                    float v = 0; // this holds current sample value
                    for (int w = border; w < window - border; w++) {
                        for (int h = border; h < window - border; h++) {
                            for (int c = 0; c < imgNumChannels; c++) {
                                v = data[c + imgNumChannels * (w * window + h)];
                                if (imgNumChannels == 3) {
                                    if (!imgFloatSample) {
                                        // numerical issues on 32bit integer
                                        v = v >= 0.9999999 ? 0.9999999 : v;
                                        v = (v + 1) * pow(2, imgBitsPerSample - 1);
                                    }
                                    else v = (v + 1) / 2;
                                } else if (imgNumChannels == 1) {
                                    v = (v * stdev + mean);
                                    // numerical issues on 32bit integer
                                    v = v >= 0.9999999 ? 0.9999999 : v;
                                    if (!imgFloatSample) v *= pow(2, imgBitsPerSample);
                                }
                                
                                if (!imgFloatSample) starless(i + w, j + h, c) = v < 0 ? 0 : (uint32) v;
                                else starless(i + w, j + h, c) = v < 0 ? 0 : v;
                            }
                        }
                    }
                    
                    TF_DeleteTensor(inputTensor);
                    TF_DeleteTensor(outputTensor);
                    
                    inputVals.clear();
                    
					
					// this counter should show the progress of transformation
					// however, since PI freezes, there is no use in it for now
                    counter++;
                    // std::cout << std::setw(3) << (int) (100 * counter / max) << "% finished\r" << std::flush;
                }
            }
            
			
			// crop the image back to its original size
			// if we want mask, return difference between initial and output images
            if (instance.p_mask) {
                GenericImage<P> mask(finalWidth, finalHeight, image.ColorSpace());
                mask = transformed - starless;
                image = mask.CropBy(-left, -top, -right, -bottom);
            } else {
                image = starless.CropBy(-left, -top, -right, -bottom);
            }
            
			
			// some cleaning
            TF_CloseSession(sess, status);
            TF_DeleteSession(sess, status);
            TF_DeleteSessionOptions(options);
            TF_DeleteGraph(graph);
            TF_DeleteStatus(status);
            
            Console().WriteLn("Done!");
        }
    };

    bool StarNetInstance::ExecuteOn(View& view) {
        AutoViewLock lock(view);
        
        ImageVariant image = view.Image();
        if (image.IsComplexSample()) return false;
        
        StandardStatus status;
        image.SetStatusCallback(&status);
        
        Console().EnableAbort();
        
        if (image.IsFloatSample())
            switch (image.BitsPerSample()) {
                case 32: StarNetEngine::Apply(static_cast<Image&>(*image), view, *this); break;
                case 64: StarNetEngine::Apply(static_cast<DImage&>(*image), view, *this); break;
            }
        else
            switch (image.BitsPerSample()) {
                case  8: StarNetEngine::Apply(static_cast<UInt8Image&>(*image), view, *this); break;
                case 16: StarNetEngine::Apply(static_cast<UInt16Image&>(*image), view, *this); break;
                case 32: StarNetEngine::Apply(static_cast<UInt32Image&>(*image), view, *this); break;
            }
        return true;
    }

    void* StarNetInstance::LockParameter(const MetaParameter* p, size_type /*tableRow*/) {
        if (p == TheStrideParameter)
            return &p_stride;
        if (p == TheMaskParameter)
            return &p_mask;

        return nullptr;
    }

    bool StarNetInstance::AllocateParameter(size_type sizeOrLength, const MetaParameter* p, size_type tableRow) {
        return true;
    }

    size_type StarNetInstance::ParameterLength(const MetaParameter* p, size_type tableRow) const {
        return 0;
    }
}