#pragma once
#include <iostream>
#include <list>
#include <thread>
#include <future>
#include <functional>
#include <optional>
#include <atomic>
#include <variant>
#include <memory>
#include <type_traits>
#include <any>


namespace NesesUtils
{

			/*
			pooldan thread weak pointer al,
			callable set ederek baslat, nesesthreadi arguman olarak callable'a gönder ve diziye al

			*/

	class NesesThread 
	{

	private:
		const std::string name_;
		std::atomic<bool> stopflag_{ false };
		std::atomic<bool> isdone_{ false };
		bool IsDeleted{ false };


	private:

		std::thread th_;
		NesesThread(const std::string& name) : name_(name)
		{
			
		}

	public:	

		const std::string GetName()
		{
			return name_;
		}

		bool GetStopFlag() const
		{
			return stopflag_.load();
		}

		void SetStopFlag(bool stopflag)
		{
			stopflag_.store(stopflag);
		}

		void SetIsDone(bool isdone)
		{
			isdone_ = isdone;
		}

		bool GetIsDone() const
		{
			return isdone_.load();
		}


	public:

		~NesesThread()
		{
			Stop();
		}

		// Delete copy constructor and copy assignment
		NesesThread(const NesesThread&) = delete;
		NesesThread& operator =(const NesesThread&) = delete;


		// Move constructor and move assignment
		NesesThread(NesesThread&& other) noexcept
			: name_(other.name_), th_(std::move(other.th_))
		{		
			stopflag_ = other.stopflag_.load();
			isdone_ = other.isdone_.load();
		}

		NesesThread& operator=(NesesThread&& other) noexcept
		{
			if (this != &other)
			{
				Stop();
				th_ = std::move(other.th_);
			}
			return *this;
		}
		//***************************************


		template<typename Function, typename... Args>
		void Start(Function&& f, Args&& ... args)
		{
			Stop();
			th_ = std::thread(std::forward<Function>(f), std::forward<Args>(args)...);
		}

		void Stop()
		{
			SetStopFlag(true);
			if (th_.joinable())
			{
				try
				{
					th_.join();
				}
				catch (const std::exception&)
				{

				}
#ifdef _DEBUG 
				std::cout << "thread: " << name_ << " joined" << std::endl;
#endif				
			}

			SetStopFlag(false);
			SetIsDone(false);
		}


		template<typename T>
		friend class ThreadManager;

	};


	/* Type Traits */

	template<typename T, typename = std::void_t<>>
	struct HasStop : std::false_type {};

	template<typename T>
	struct HasStop<T, std::void_t<decltype(std::declval<T>().Stop())>> : std::true_type {};


	template<typename T, typename = std::void_t<>>
	struct HasSetStopFlag : std::false_type {};

	template <typename T>
	struct HasSetStopFlag <T, std::void_t<decltype(std::declval<T>().SetStopFlag(std::declval<bool>()))>> : std::true_type {};


	template <typename T>
	struct IsStopable_v
	{
		static constexpr bool value = HasStop<T>::value && HasSetStopFlag<T>::value;
	};


	/**************************/



	template <typename T>
	class ThreadManager
	{
	private:

		std::mutex mtx;
		std::list<std::shared_ptr<T>> tasks;
		const unsigned int MaxTaskCount = 3;

		void RemoveFinished()
		{
			//std::cout << Count() << " tasks count" << std::endl;

			for (auto& task : tasks)
			{
				if (task == nullptr)
				{
					std::cout << "task is null" << std::endl;
					continue;
				}
				if (task->GetIsDone())
				{
					task->SetStopFlag(true);
				}
			}

			for (auto& task : tasks)
			{
				if (task == nullptr)
				{
					std::cout << "task is null" << std::endl;
					continue;
				}
				if (task->GetIsDone())
				{
					task->Stop();
					task->IsDeleted = true;
					//std::cout << "task is marked for deletion : " << task->GetName() << std::endl;
				}
			}


			auto it = tasks.begin();
			while (it != tasks.end())
			{
				if ((*it)->IsDeleted)
				{
					std::cout << "erasing " << (*it)->GetName() << std::endl;
					it = tasks.erase(it);
				}
				else
					++it;
			}
		}


		/// yeni iş açıldıkça eskileri temizleyecek
		template<typename U>
		bool AddNewTask(U newTask)
		{
			std::lock_guard<std::mutex> lock(mtx);
			RemoveFinished();
			if (tasks.size() <= MaxTaskCount)
			{
				tasks.emplace_back(newTask);

				return true;
			}
			else
				return false;
		}

	public:
		ThreadManager()
		{
			static_assert(IsStopable_v<T>::value, "Type T must have member functions named 'Stop()' and 'SetStopFlag'");
		}
		~ThreadManager()
		{
			Stop();
			std::cout << "ThreadManager instance destroyed." << std::endl;
		}


		// kopyala-ma
		ThreadManager(const ThreadManager&) = delete;
		ThreadManager& operator =(const ThreadManager&) = delete;

		void Stop()
		{
			for (auto& th : tasks)
			{
				th->SetStopFlag(true);
			}

			for (auto& th : tasks)
			{
				th->Stop();
			}

			tasks.clear();
		}

		const std::size_t Count()
		{
			return tasks.size();
		}

		
		std::shared_ptr<NesesThread> GetNew(const std::string& name = "")   
		{
				std::shared_ptr<NesesThread> newTask(new NesesThread(name));
				if (AddNewTask(newTask))
				{
					return newTask;
				}
			return nullptr;

		}
	};

}