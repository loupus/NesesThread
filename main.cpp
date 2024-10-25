#include <iostream>
#include <vector>
#include "Application.hpp"

using namespace NesesUtils;


    using NesesTaskPtr = std::shared_ptr<NesesTask<BackObject>>;
    using NesesThreadPtr = std::shared_ptr<NesesThread>;
class testme
{
    public:
    void Worker(NesesThreadPtr nthread)
    {
       
        while(true)
        {
            std::cout << "doing some long time worker stuff" << std::endl;

            if(nthread->GetStopFlag())
                break;

           std::this_thread::sleep_for(std::chrono::seconds(2));
        }
        nthread->SetIsDone(true);
        std::cout << "exiting worker " << std::endl;
    }

    BackObject FireAndForget(NesesTaskPtr ntask)
    {
         BackObject back;
        for(int i =0; i<100; i++)
        {
            if(i % 10 == 0)
                ntask->cbProgress_.invoke(i,ntask->GetName());
            
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
        ntask->cbCompleted_.invoke(ntask->GetName());
        back.Success = true;
        return back;
    }

};

void onProgress(int p, std::string& str)
{
    std::cout << str << " progress % " << p << std::endl;
}

void onCompleted(std::string& str)
{
    std::cout << str << " completed" << std::endl;
}

int main()
{
    std::cout << "begining" << std::endl;

    testme t;
    
    CallBack<int,std::string&> cbProgress;
    CallBack<std::string&> cbCompleted;

    cbProgress.setCallback(onProgress);
    cbCompleted.setCallback(onCompleted);

    auto nthread_ =  Application::GetInstance().NewWorker("worker");
    nthread_->Start(&testme::Worker, t, nthread_);
    
    std::vector<NesesTaskPtr> tasks;
    for(int i = 1; i< 4; i++)
    {
        auto taskp = Application::GetInstance().NewTask("task " + std::to_string(i));
        taskp->Set(&testme::FireAndForget, &t, taskp);
        taskp->cbProgress_ = cbProgress;
        taskp->cbCompleted_ = cbCompleted;

        if (Application::GetInstance().EnqueuTask(taskp))
             tasks.emplace_back(taskp);
        else
            std::cout << "failed to enqueu task: " << taskp->GetName() << std::endl; 
    }

     std::this_thread::sleep_for(std::chrono::seconds(120));

    // finish
     BackObject back;
     for(auto& stask : tasks)
     {
        back = stask->GetFuture().get();
        if(!back.Success)
            std::cout << back << std::endl;
        back.Reset();
     }

    nthread_->Stop();

    std::cout << "ending" << std::endl;

    return 0;
}