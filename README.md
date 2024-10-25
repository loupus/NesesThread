NesesThread:

   * Get a pointer by 
```
        auto nthread_ =  Application::GetInstance().NewWorker("worker");
```

   * Let your function have arguments any type and number, send the thread pointer as argument  
   Call start by sending callable and arguments. Object ownership will also be shared by threadmanager

```
        nthread_->Start(&myclass::memberfunction, objectpointer, nesesthreadptr,  args... ); 
```

  * If you want to make use of stopping mechanism make sure you implement the break by checking the stopflag. Dont forget to set IsDone flag before you exit your thread function, so thread manager will remove it

```
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

```
       nesesthreadptr.SetStopFlag(true);
```

////////////////////////////////////////////////////////

NesesTask:
    * Similar usage with NesesThread, except that, you set Callable and arguments on Set function and  you start running task by enqueuing
    ```

        auto nesestaskptr = Application::GetInstance().NewTask("mytask");
        nesestaskptr->Set(&myclass::memberfunction, objectpointer, taskp);
        Application::GetInstance().EnqueuTask(nesestaskptr);

    ```
    
    * NesesTask has also internal progress and completed callbacks. You can create and set these before you enqueue.

     ```
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

    ```
    auto future_ = nesestaskptr.GetFuture();
    ...
    auto ret = future_.get();

     ```
        