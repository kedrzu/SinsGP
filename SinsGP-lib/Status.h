#ifndef STATUS_H
#define STATUS_H

namespace SinsGP {
class Status
{
public:
    Status();
    int getRun() { return mRun; }
    int getModel() { return mModel; }
    boost::filesystem::path getRunPath() { return mRunPath; }
    boost::filesystem::path getModelPath() { return mModelPath; }
    int nextRun() {
        mModel = 0;
        ++mRun;
        updatePaths();
        return mRun;
    }
    int nextModel() {
        ++mModel;
        updatePaths();
        return mModel;
    }
    void setRun(unsigned i){
        mRun = i;
        updatePaths();
    }
    void setModel(unsigned i){
        mModel = i;
        updatePaths();
    }

private:
    unsigned mRun;
    unsigned mModel;
    boost::filesystem::path mRunPath;
    boost::filesystem::path mModelPath;
    void updatePaths() {
        mRunPath = boost::filesystem::path("run-"+Beagle::int2str(mRun));
        mModelPath = mRunPath / boost::filesystem::path("model-"+Beagle::int2str(mModel));
    }
};
}

#endif // STATUS_H
