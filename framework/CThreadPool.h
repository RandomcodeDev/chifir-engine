/// @file CThreadPool.h
/// @brief This file defines the thread pool class
/// @copyright (c) 2024 Randomcode Developers. See LICENSE.

#pragma once

#include "Framework.h"

struct WorkerThread;

class CThreadPool
{
  public:
	CThreadPool() = default;
	~CThreadPool();

	using TaskFunc = std::function<bool(void*)>;

	/// @brief Add a task to the queue
	void AddWork(TaskFunc task);

  private:
	std::queue<TaskFunc> m_tasks;
	std::vector<WorkerThread> m_workers;
};
