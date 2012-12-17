/**
 * \file MobileHandler.cpp
 * \brief Mobile handler
 * \author vsalo
 */

#include "AppMgr/MobileHandler.h"
#include "JSONHandler/JSONHandler.h"
#include "LoggerHelper.hpp"

namespace NsAppManager
{

    log4cplus::Logger MobileHandler::mLogger = log4cplus::Logger::getInstance(LOG4CPLUS_TEXT("AppMgrCore"));

    /**
     * \brief Returning class instance
     * \return class instance
     */
    MobileHandler &MobileHandler::getInstance()
    {
        static MobileHandler instance;
        return instance;
    }

    /**
     * \brief send a message via associated handler
     * \param message message to send
     * \param connectionID id of a connection associated with application that sent the message
     * \param sessionId id of a session associated with application that sent the message
     */
    void MobileHandler::sendRPCMessage(const NsAppLinkRPC::ALRPCMessage *message, int appId)
    {
        if(mJSONHandler)
        {
            LOG4CPLUS_INFO_EXT(mLogger, " Sending a message " << message->getMethodId()
                << " of the type " << message->getMessageType()
                << " with the application id " << appId
                << " to a mobile side");
            mJSONHandler->sendRPCMessage(message, appId);
        }
        else
        {
            LOG4CPLUS_ERROR_EXT(mLogger, " MobileHandler is null!");
        }
    }

    /**
     * \brief set mobile Json handler
     * \param handler mobile Json handler instance
     */
    void MobileHandler::setJsonHandler(JSONHandler *handler)
    {
        if(!handler)
        {
            LOG4CPLUS_ERROR_EXT(mLogger, "A null pointer is being assigned - is this the intent?");
            return;
        }
        mJSONHandler = handler;
    }

    /**
     * \brief get mobile Json handler
     * \return mobile Json handler instance
     */
    JSONHandler *MobileHandler::getJsonHandler() const
    {
        if(!mJSONHandler)
        {
            LOG4CPLUS_ERROR_EXT(mLogger, "JSON Mobile handler hasn't yet been assigned, but an attempt to retrieve it has been made! Face a core dump soon...(((");
        }
        return mJSONHandler;
    }

    /**
     * \brief Default class constructor
     */
    MobileHandler::MobileHandler()
        :mJSONHandler(0)
    {
    }

    /**
     * \brief Copy constructor
     */
    MobileHandler::MobileHandler(const MobileHandler &)
        :mJSONHandler(0)
    {
    }

}
