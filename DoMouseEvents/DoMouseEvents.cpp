// DoMouseEvents.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <boost/asio.hpp>
#include <boost/asio/deadline_timer.hpp>
#include <boost/bind.hpp>

#include <chrono>
#include <iostream>

class Pooper
{
    boost::asio::io_context & m_ioContext;
    boost::asio::steady_timer m_timer;
    int m_count;

public:
    Pooper(boost::asio::io_context & ioContext)
        :
        m_ioContext(ioContext)
      , m_timer(boost::asio::steady_timer(ioContext))
      , m_count(0)
    {
    }

    void Run()
    {
        m_timer.expires_from_now(std::chrono::seconds(3));
        m_timer.async_wait(boost::bind(&Pooper::OnTimerExpired, this, boost::asio::placeholders::error));
    }

    void OnTimerExpired(const boost::system::error_code & error)
    {
        if (error.failed())
        {
            std::cerr << "boost error: Failed" << std::endl;
        }

        if (m_count >= 50)
        {
            return;
        }

        std::cout << "Poop" << m_count << std::endl;
        ++m_count;

        try
        {
            // Move the mouse
            int x = 500;
            int y = 500;
            if (!::SetCursorPos(x, y))
            {
                int errorCode = GetLastError();
                std::cerr << "SetCursorPos error: " << errorCode << std::endl;
            }

            // Left button down
            x = 65536 * 500;
            y = 65536 * 500;
            ::mouse_event(MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_LEFTDOWN, x, y, NULL, NULL);

            // Left button up
            x = 65536 * 500;
            y = 65536 * 500;
            ::mouse_event(MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_LEFTUP, x, y, NULL, NULL);

            m_timer.expires_from_now(std::chrono::seconds(3));
            m_timer.async_wait(boost::bind(&Pooper::OnTimerExpired, this, boost::asio::placeholders::error));
        }
        catch (std::exception &)
        {
            std::cerr << "An exception occured." << std::endl;
        }
    }
};

int main()
{
    boost::asio::io_context ioContext;
    auto pooper = Pooper(ioContext);
    pooper.Run();
    ioContext.run();

    std::cout << "Exited..." << std::endl;
}

