#pragma once

#include "TOP_CPlusPlusBase.h"
#include "libfreenect.hpp"
#include <atomic>
#include <vector>
#include <mutex>
#include <thread>

using namespace TD;

class MyFreenectDevice : public Freenect::FreenectDevice {
public:
    MyFreenectDevice(freenect_context* ctx, int index,
                     std::atomic<bool>& rgbFlag, std::atomic<bool>& depthFlag,
                     std::atomic<bool>& irFlag);
    void VideoCallback(void* rgb, uint32_t) override;
    void DepthCallback(void* depth, uint32_t) override;
    void IRCallback(void* ir, uint32_t);
    bool getRGB(std::vector<uint8_t>& out);
    bool getDepth(std::vector<uint16_t>& out);
    bool getIR(std::vector<uint8_t>& out);
private:
    std::atomic<bool>&    rgbReady;
    std::atomic<bool>&    depthReady;
    std::atomic<bool>&    irReady;
    std::vector<uint8_t>  rgbBuffer;
    std::vector<uint16_t> depthBuffer;
    std::vector<uint8_t>  irBuffer;
    std::mutex            mutex;
    bool                  hasNewRGB;
    bool                  hasNewDepth;
    bool                  hasNewIR;
};

class FreenectTOP : public TOP_CPlusPlusBase {
public:
    FreenectTOP(const OP_NodeInfo* info, TOP_Context* context);
    virtual ~FreenectTOP();

    void getGeneralInfo   (TOP_GeneralInfo* ginfo, const OP_Inputs* inputs, void*) override;
    void execute          (TOP_Output* output, const OP_Inputs* inputs, void*) override;
    void setupParameters  (OP_ParameterManager* manager, void*) override;
    void pulsePressed     (const char* name, void*) override;

private:
    const OP_NodeInfo*        myNodeInfo;
    TOP_Context*              myContext;
    freenect_context*         freenectContext;
    MyFreenectDevice*         device;

    std::atomic<bool>         firstRGBReady{false};
    std::atomic<bool>         firstDepthReady{false};
    std::atomic<bool>         firstIRReady{false};

    std::vector<uint8_t>      lastRGB;
    std::vector<uint16_t>     lastDepth;
    std::vector<uint8_t>      lastIR;

    // background event thread control
    std::atomic<bool>         runEvents{false};
    std::thread               eventThread;
    
    bool initDevice();
    void cleanupDevice();
    
    std::mutex freenectMutex;
    
};
