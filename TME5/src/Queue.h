#pragma once

#include <cstdlib>
#include <cstring>
#include <mutex>
#include <condition_variable>
#include <cstring>
#include <mutex>
namespace pr {

// MT safe version of the Queue, non blocking.
// Based on circular buffer.
// Store pointers to T, not T itself ; consumer is responsible for deleting them.
// nullptr is returned by pop if the queue is empty.
// push returns false if the queue is full.
template <typename T>
class Queue {
	T ** tab_;
	const size_t allocsize_;
	size_t begin_;
	size_t size_;
	mutable std::mutex m_;
	std::condition_variable cv_;
    bool isBlocking_ = true;

	// fonctions private, sans protection mutex
	bool empty() const {
		return size_ == 0;
	}
	bool full() const {
		return size_ == allocsize_;
	}
public:
	Queue(size_t size=10) :allocsize_(size), begin_(0), size_(0) {
		tab_ = new T*[size];
		// zero-initialize, not strictly necessary
		memset(tab_, 0, size * sizeof(T*));
	}
	size_t size() const {
		std::unique_lock<std::mutex> lg(m_);
		return size_;
	}
	T* pop() {
        T* ret = nullptr;
        {
            std::unique_lock<std::mutex> lck(m_);
            while (empty() && isBlocking_) {
                cv_.wait(lck);
            }
            if (empty()) {
                // soit non bloquant, soit réveil spurieux alors que vide et !isBlocking_
                return nullptr;
            }
            ret = tab_[begin_];
            tab_[begin_] = nullptr;
            --size_;
            begin_ = (begin_ + 1) % allocsize_;
        }
        // Réveiller un producteur éventuel si on vient de libérer une case.
        cv_.notify_all();
        return ret;
    }
	bool push(T* elt) {
        {
            std::unique_lock<std::mutex> lg(m_);
            while (full() && isBlocking_) {
                cv_.wait(lg);
            }
            if (full()) {
                // non bloquant et plein
                return false;
            }
            tab_[(begin_ + size_) % allocsize_] = elt;
            ++size_;
        }
        // Réveiller un consommateur éventuel si on vient d'ajouter un élément.
        cv_.notify_all();
        return true;
    }
	~Queue() {
        // On libère les éléments restants si le consommateur ne les a pas récupérés.
        // Hypothèse: T* a été alloué par new T(...)
        for (size_t i = 0; i < size_; ++i) {
            const size_t idx = (begin_ + i) % allocsize_;
            delete tab_[idx];
        }
        delete[] tab_;
    }
	void setBlocking(bool b) {
        std::unique_lock<std::mutex> lg(m_);
        isBlocking_ = b;
        if (!isBlocking_) {
            cv_.notify_all();
        }
    }
};

} /* namespace pr */
