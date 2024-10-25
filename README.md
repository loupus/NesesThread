VERY SIMPLE THREAD MANAGEMENT

We have here two wrapper classes around std::thread, which, I hope, will make life easier. Of course there are many pro wrappers, the difference is, these are worked out by Neses Sofware. 
NesesThread is for worker loops, NesesTask is for run and forget type functions. Ownership is shared between thread pools and caller, so carefull you be young jedi, shared_ptr may cause blocks 
if they depend each other! We didnt like weak_ptr but you may use it at caller side. 

The purpose of these is, to get control over all threads of a project from a single singleton application object. Thread without control is trouble. 

All header files in accordance with cpp17, so add headers to your project and use as you wish

NesesSofware is wonderful. 



**NesesThread:**

   * Get a pointer by 

  ```c++
        auto nesesthreadptr =  Application::GetInstance().NewWorker("worker");
  ```


   * Let your function have arguments any type and number, send the thread pointer as argument  
   Call start by sending callable and arguments. Object ownership will also be shared by threadmanager


```c++
        nesesthreadptr->Start(&myclass::memberfunction, objectpointer, nesesthreadptr,  args... ); 
```


  * If you want to make use of stopping mechanism make sure you implement the break by checking the stopflag. Dont forget to set IsDone flag before you exit your thread function, so thread manager will remove it


```c++
    ReturnValue SomeFunction(std::shared_ptr<NesesThread> nesesthreadptr)
    {
        while(true)
        {
            ...
            if( nesesthreadptr.GetStopFlag())
                break;
        }
        ...
        nesesthreadptr->SetIsDone(true);
    }
```

Then you can set stop whenever you like


  ```c++
       nesesthreadptr.SetStopFlag(true);
  ```



**NesesTask:**

* Similar usage with NesesThread, except that, you set Callable and arguments on Set function and  you start running task by enqueuing

   

```c++
        auto nesestaskptr = Application::GetInstance().NewTask("mytask");
        nesestaskptr->Set(&myclass::memberfunction, objectpointer, taskp);
        Application::GetInstance().EnqueuTask(nesestaskptr);
```

  
    
* NesesTask has also internal progress and completed callbacks. You can create and set these before you enqueue.

```c++
         void onProgress(int p, std::string& str)
        {
            std::cout << str << " progress % " << p << std::endl;
        }

        void onCompleted(std::string& str)
        {
            std::cout << str << " completed" << std::endl;
        }

    CallBack<int,std::string&> cbProgress;
    CallBack<std::string&> cbCompleted;

    cbProgress.setCallback(onProgress);
    cbCompleted.setCallback(onCompleted);

    nesestaskptr->cbProgress_ = cbProgress;
    nesestaskptr->cbCompleted_ = cbCompleted;
```
       
  

* You can also get future from task pointer after you set it;

   
```c++
        auto future_ = nesestaskptr.GetFuture();
        ...
        auto ret = future_.get();
```

   
        