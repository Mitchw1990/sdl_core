/**
 * \file AppMgr.h
 * \brief Application manager main component
 * \author vsalo
 */

#ifndef APPMGR_H
#define APPMGR_H

#include "JSONHandler/IRPCMessagesObserver.h"
#include "JSONHandler/IRPC2CommandsObserver.h"

class JSONHandler;
class ALRPCMessage;
class JSONRPC2Handler;

namespace log4cplus
{
    class Logger;
}

namespace NsRPC2Communication
{
    class RPC2Command;
}

namespace NsAppManager
{

    class AppMgrRegistry;
    class AppMgrCore;
    class AppFactory;


    /**
    * \brief a main app manager class which acts like container for other classes
    */
    class AppMgr: public IRPCMessagesObserver, public IRPC2CommandsObserver
    {
    public:

        /**
         * \brief Returning class instance
         * \return class instance
         */
        static AppMgr& getInstance();

        /**
         * \brief callback to proceed received mobile message
         * \param message the received message
         * \param sessionID an id of a session associated with the application which sends message
         */
        virtual void onMessageReceivedCallback( NsAppLinkRPC::ALRPCMessage * message, unsigned char sessionID );

        /**
         * \brief callback to proceed received RPC2 command
         * \param command the received command
         */
        virtual void onCommandReceivedCallback( NsRPC2Communication::RPC2Command * command );

        /**
         * \brief Sets Json mobile handler instance
         * \param handler Json mobile handler
         */
        void setJsonHandler(JSONHandler* handler);

        /**
         * \brief Sets Json RPC2 handler instance
         * \param handler Json RPC2 handler
         */
        void setJsonRPC2Handler(JSONRPC2Handler* handler);

        /**
         * \brief method to execute threads.
         */
        void executeThreads();

    private:

        /**
         * \brief Default class destructor
         */
        virtual ~AppMgr();

        /**
         * \brief Copy constructor
         */
        AppMgr(const AppMgr&);

        /**
         * \brief Default class constructor
         */
        AppMgr();

        static log4cplus::Logger mLogger;
    };

} // namespace NsAppManager

#endif // APPMGR_H
