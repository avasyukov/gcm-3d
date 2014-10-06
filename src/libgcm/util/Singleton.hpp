/*
 * File:   Singleton.h
 * Author: Alexey Ermakov
 *
 * Created on March 8, 2014, 8:51 PM
 */

#ifndef SINGLETON_H
#define    SINGLETON_H

#include <utility>
namespace gcm
{
    template <typename T>
    class Singleton
    {
    public:

        template <typename... Args>
        static T& getInstance(Args... args)
        {
            if (!instance_) {
                instance_ = new T(args...);
            }

            return *instance_;
        }

        static void destroyInstance()
        {
            delete instance_;
            instance_ = nullptr;
        }

    private:
        static T* instance_;

    protected:
        Singleton()
        {
        }
        Singleton(const Singleton& s);
        virtual ~Singleton()
		{

		}
        void operator=(const Singleton& s);
    };
}

template<typename T>
T* gcm::Singleton<T>::instance_ = nullptr;


#endif    /* SINGLETON_H */

