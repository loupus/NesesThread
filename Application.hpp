#pragma once
#include <iostream>
#include <thread>
#include <string>
#include <vector>
#include <memory>
#include "BackObject.hpp"
#include "NesesThread.hpp"
#include "NesesTask.hpp"

namespace NesesUtils
{
// singleton or monostate
//Meyers Singleton
//https://medium.com/@caglayandokme/further-enhancing-the-singleton-pattern-in-c-8278b02b1ac7

	class Application
	{
	private:
		

		Application() 
		{
			std::cout << "application instance created." << std::endl;
		}
		~Application()
		{
			//ReleaseInstance();
			std::cout << "application instance destroyed." << std::endl;
		}

		ThreadManager<NesesThread> tm;
		TaskPool<BackObject> tp;
		
	public:

		

		// kopyalama
		Application(const Application&) = delete;
		Application& operator =(const Application&) = delete;


		static Application& GetInstance()
		{
			static Application _instance;
			return _instance;
		}

		void ReleaseInstance()
		{			
			StopThreads();
			std::cout << "application instance released" << std::endl;
		}

		void Init()
		{
			
		}
		
		std::shared_ptr<NesesThread> NewWorker(const std::string& name="")
		{
			return tm.GetNew(name);
		}

/* 		template<typename Func, typename... Args>
		std::shared_ptr<NesesTask<BackObject>> NewTask(const std::string& name, Func&& func, Args&& ...args)
		{
			return tp.GetNew(name, std::forward<Func>(func), std::forward<Args>(args)...);
		}
 */
		
		std::shared_ptr<NesesTask<BackObject>> NewTask(const std::string& name)
		{
			return tp.GetNew(name);
		}

		bool EnqueuTask(std::shared_ptr < NesesTask < BackObject>>& task)
		{
			return tp.Enqueue(task);
		}

		void StopThreads()
		{
			tm.Stop();
			tp.Stop();
		}
	};

}

