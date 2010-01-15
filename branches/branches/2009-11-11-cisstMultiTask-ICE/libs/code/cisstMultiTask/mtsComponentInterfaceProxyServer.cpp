/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id: mtsComponentInterfaceProxyServer.cpp 145 2009-03-18 23:32:40Z mjung5 $

  Author(s):  Min Yang Jung
  Created on: 2010-01-12

  (C) Copyright 2010 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

//#include <cisstCommon/cmnAssert.h>
#include <cisstOSAbstraction/osaSleep.h>
#include <cisstMultiTask/mtsComponentInterfaceProxyServer.h>

//#include <sstream>

CMN_IMPLEMENT_SERVICES(mtsComponentInterfaceProxyServer);

#define ComponentInterfaceProxyServerLogger(_log) IceLogger->trace("mtsComponentInterfaceProxyServer", _log)
#define ComponentInterfaceProxyServerLoggerError(_log1, _log2) {\
        std::string s("mtsComponentInterfaceProxyServer: ");\
        s += _log1; s+= _log2;\
        IceLogger->error(s); }

//-----------------------------------------------------------------------------
//  Constructor, Destructor, Initializer
//-----------------------------------------------------------------------------
mtsComponentInterfaceProxyServer::~mtsComponentInterfaceProxyServer()
{
    //// Add any resource clean-up related methods here, if any.
    //TaskManagerMapType::iterator it = TaskManagerMap.begin();
    //for (; it != TaskManagerMap.end(); ++it) {
    //    delete it->second;
    //}
}

//-----------------------------------------------------------------------------
//  Proxy Start-up
//-----------------------------------------------------------------------------
void mtsComponentInterfaceProxyServer::Start(mtsComponentProxy * proxyOwner)
{
    // Initialize Ice object.
    IceInitialize();
    
    if (!InitSuccessFlag) {
        ComponentInterfaceProxyServerLogger("Initialization failed");
        return;
    }

    // Create a worker thread here and returns immediately.
    ThreadArgumentsInfo.ProxyOwner = proxyOwner;
    ThreadArgumentsInfo.Proxy = this;
    ThreadArgumentsInfo.Runner = mtsComponentInterfaceProxyServer::Runner;

    // Note that a worker thread is created but is not yet running.
    WorkerThread.Create<ProxyWorker<mtsComponentProxy>, ThreadArguments<mtsComponentProxy>*>(
        &ProxyWorkerInfo, &ProxyWorker<mtsComponentProxy>::Run, &ThreadArgumentsInfo,
        // Set the name of this thread as CIPS which means Component 
        // Interface Proxy Server. Such a very short naming rule is
        // because sometimes there is a limitation of the total number 
        // of characters as a thread name on some systems (e.g. LINUX RTAI).
        "CIPS");
}

void mtsComponentInterfaceProxyServer::StartServer()
{
    Sender->Start();

    // This is a blocking call that should be run in a different thread.
    IceCommunicator->waitForShutdown();
}

void mtsComponentInterfaceProxyServer::Runner(ThreadArguments<mtsComponentProxy> * arguments)
{
    mtsComponentInterfaceProxyServer * ProxyServer = 
        dynamic_cast<mtsComponentInterfaceProxyServer*>(arguments->Proxy);
    if (!ProxyServer) {
        CMN_LOG_RUN_ERROR << "mtsComponentInterfaceProxyServer: Failed to create a proxy server." << std::endl;
        return;
    }

    // Set owner of this proxy object
    ProxyServer->SetProxyOwner(arguments->ProxyOwner);

    ProxyServer->GetLogger()->trace("mtsComponentInterfaceProxyServer", "Proxy server starts.....");

    try {
        ProxyServer->SetAsActiveProxy();
        ProxyServer->StartServer();
    } catch (const Ice::Exception& e) {
        std::string error("mtsComponentInterfaceProxyServer: ");
        error += e.what();
        ProxyServer->GetLogger()->error(error);
    } catch (const char * msg) {
        std::string error("mtsComponentInterfaceProxyServer: ");
        error += msg;
        ProxyServer->GetLogger()->error(error);
    }

    ProxyServer->GetLogger()->trace("mtsComponentInterfaceProxyServer", "Proxy server terminates.....");

    ProxyServer->Stop();
}

void mtsComponentInterfaceProxyServer::Stop()
{
    BaseServerType::Stop();

    Sender->Stop();
}

//-------------------------------------------------------------------------
//  Event Handlers (Server -> Client)
//-------------------------------------------------------------------------
void mtsComponentInterfaceProxyServer::TestReceiveMessageFromClientToServer(const std::string & str) const
{    
    std::cout << "Test: Client -> Server: Received: " << str << std::endl;
}

void mtsComponentInterfaceProxyServer::ReceiveAddClient(
    const ConnectionIDType & connectionID, ComponentInterfaceClientProxyType & clientProxy)
{
    if (!this->AddProxyClient(connectionID, clientProxy)) {
        ComponentInterfaceProxyServerLoggerError("ReceiveAddClient: failed to add proxy client: ", connectionID);
        return;
    }

    IceLogger->trace("ReceiveAddClient: added proxy client: ", connectionID);
}

/*
bool mtsComponentInterfaceProxyServer::ReceiveUpdateTaskManagerClient(
    const ConnectionIDType & connectionID, const mtsTaskManagerProxy::TaskList& localTaskInfo)
{
    const TaskManagerIDType taskManagerID = localTaskInfo.taskManagerID;

    ConnectionIDMap.insert(make_pair(taskManagerID, connectionID));
    
    TaskManagerClient * taskManagerClient = GetTaskManagerByConnectionID(connectionID);
    if (!taskManagerClient) {
        ComponentInterfaceProxyServerLoggerError("[UpdateTaskManagerClient] Cannot find a task manager: ", connectionID);
        return false;
    }

    ComponentInterfaceProxyServerLogger("Adding tasks from connection id: " + connectionID);

    bool success = true;
    std::string taskName;
    mtsTaskManagerProxy::TaskNameSeq::const_iterator it = localTaskInfo.taskNames.begin();    
    for (; it != localTaskInfo.taskNames.end(); ++it) {
        taskName = *it;
        mtsTaskGlobal * newTask = new mtsTaskGlobal(taskName, taskManagerID);

        if (!taskManagerClient->AddTaskGlobal(newTask)) {
            ComponentInterfaceProxyServerLoggerError("[UpdateTaskManagerClient] Add failed: ", taskName);
            success = false;
        } else {
            if (!TaskManagerMapByTaskName.AddItem(taskName, taskManagerClient)) {
                ComponentInterfaceProxyServerLoggerError("[UpdateTaskManagerClient] Add failed (duplicate task name): ", taskName);
                success = false;
            } else {
                ComponentInterfaceProxyServerLogger("Successfully added a task: " + taskName);
            }
        }

        ComponentInterfaceProxyServerLogger(newTask->ShowTaskInfo());
    }

    return success;
}
*/

//-------------------------------------------------------------------------
//  Event Generators (Event Sender) : Server -> Client
//-------------------------------------------------------------------------
void mtsComponentInterfaceProxyServer::SendTestMessageFromServerToClient(const std::string & str)
{
    IceLogger->trace("ComponentInterface: Server", ">>>>> SEND: SendMessageFromServerToClient");

    // iterate client map -> send message to ALL clients (broadcasts)
    ComponentInterfaceClientProxyType * clientProxy;
    ClientProxyMapType::iterator it = ClientProxyMap.begin();
    ClientProxyMapType::const_iterator itEnd = ClientProxyMap.end();
    for (; it != itEnd; ++it) {
        clientProxy = &it->second;
        try 
        {
            (*clientProxy)->TestSendMessageFromServerToClient(str);
        }
        catch (const ::Ice::Exception & ex)
        {
            std::cerr << "Error: " << ex << std::endl;
            continue;
        }
    }
}

void mtsComponentInterfaceProxyServer::OnClose()
{
    //
    //  TODO: Add OnClose() event handler.
    //

    // remove from TaskManagerMapByTaskName
    // remove from TaskManagerClient
    //RemoveTaskManagerByConnectionID();
}

/*
bool mtsComponentInterfaceProxyServer::ReceiveAddProvidedInterface(
    const ConnectionIDType & connectionID, 
    const mtsTaskManagerProxy::ProvidedInterfaceAccessInfo & providedInterfaceAccessInfo)
{
    TaskManagerClient * taskManagerClient = GetTaskManagerByConnectionID(connectionID);
    if (!taskManagerClient) {
        ComponentInterfaceProxyServerLoggerError("[AddProvidedInterface] Cannot find a task manager: ", connectionID);
        return false;
    }

    const std::string taskName = providedInterfaceAccessInfo.taskName;
    const std::string interfaceName = providedInterfaceAccessInfo.interfaceName;

    mtsTaskGlobal * taskGlobal = taskManagerClient->GetTaskGlobal(taskName);
    if (!taskGlobal) {
        ComponentInterfaceProxyServerLoggerError("[AddProvidedInterface] Cannot find a global task: ", taskName);
        return false;
    }

    bool success = taskGlobal->AddProvidedInterface(providedInterfaceAccessInfo);
    if (success) {
        ComponentInterfaceProxyServerLogger(
            "Successfully added a provided interface: " + 
            interfaceName + " @ " + taskName);
        ComponentInterfaceProxyServerLogger(taskGlobal->ShowTaskInfo());        
    } else {
        ComponentInterfaceProxyServerLoggerError(
            "[AddProvidedInterface] Failed to add a provided interface: ", 
            interfaceName + " @ " + taskName);
    }

    return success;
}

bool mtsComponentInterfaceProxyServer::ReceiveAddRequiredInterface(
    const ConnectionIDType & connectionID,
    const mtsTaskManagerProxy::RequiredInterfaceAccessInfo & requiredInterfaceAccessInfo)
{
    TaskManagerClient * taskManagerClient = GetTaskManagerByConnectionID(connectionID);
    if (!taskManagerClient) {
        ComponentInterfaceProxyServerLoggerError("[AddRequiredInterface] Cannot find a task manager: ", connectionID);
        return false;
    }

    mtsTaskGlobal * taskGlobal = taskManagerClient->GetTaskGlobal(requiredInterfaceAccessInfo.taskName);
    if (!taskGlobal) {
        ComponentInterfaceProxyServerLoggerError("[AddRequiredInterface] Cannot find a global task: ", requiredInterfaceAccessInfo.taskName);
        return false;
    }

    bool success = taskGlobal->AddRequiredInterface(requiredInterfaceAccessInfo);
    if (success) {
        ComponentInterfaceProxyServerLogger(
            "Successfully added a required interface: " + 
            requiredInterfaceAccessInfo.interfaceName + " @ " +
            requiredInterfaceAccessInfo.taskName);
        ComponentInterfaceProxyServerLogger(taskGlobal->ShowTaskInfo());        
    } else {
        ComponentInterfaceProxyServerLoggerError(
            "[AddRequiredInterface] Failed to add a required interface: ",
            requiredInterfaceAccessInfo.interfaceName + " @ " +
            requiredInterfaceAccessInfo.taskName);
    }

    return success;
}

bool mtsComponentInterfaceProxyServer::ReceiveIsRegisteredProvidedInterface(
    const ConnectionIDType & connectionID,
    const std::string & taskName, const std::string & providedInterfaceName)
{
    TaskManagerClient * taskManagerClient = TaskManagerMapByTaskName.GetItem(taskName);
    if (!taskManagerClient) {
        ComponentInterfaceProxyServerLoggerError("[IsRegisteredProvidedInterface] Cannot find a task manager with the task: ", taskName);
        return false;
    }

    mtsTaskGlobal * taskGlobal = taskManagerClient->GetTaskGlobal(taskName);
    if (!taskGlobal) {
        ComponentInterfaceProxyServerLoggerError("[IsRegisteredProvidedInterface] Cannot find a global task: ", taskName);
        return false;
    }

    return taskGlobal->IsRegisteredProvidedInterface(providedInterfaceName);
}

bool mtsComponentInterfaceProxyServer::ReceiveGetProvidedInterfaceAccessInfo(
    const ConnectionIDType & connectionID,
    const std::string & taskName, const std::string & providedInterfaceName,
    mtsTaskManagerProxy::ProvidedInterfaceAccessInfo & info)
{
    TaskManagerClient * taskManagerClient = TaskManagerMapByTaskName.GetItem(taskName);
    if (!taskManagerClient) {
        ComponentInterfaceProxyServerLoggerError("[GetProvidedInterfaceAccessInfo] Cannot find a task manager: ", connectionID);
        return false;
    }

    mtsTaskGlobal * taskGlobal = taskManagerClient->GetTaskGlobal(taskName);
    if (!taskGlobal) {
        ComponentInterfaceProxyServerLoggerError("[GetProvidedInterfaceAccessInfo] Cannot find a global task: ", taskName);
        return false;
    }

    return taskGlobal->GetProvidedInterfaceAccessInfo(providedInterfaceName, info);
}

void mtsComponentInterfaceProxyServer::ReceiveNotifyInterfaceConnectionResult(
    const ConnectionIDType & connectionID,
    const bool isServerTask, const bool isSuccess,
    const std::string & userTaskName,     const std::string & requiredInterfaceName,
    const std::string & resourceTaskName, const std::string & providedInterfaceName)
{
    if (!isSuccess) {
        std::stringstream buf;
        buf << (isServerTask ? "Server task: " : "Client task: ")
            << resourceTaskName << " : " << providedInterfaceName << " - "
            << userTaskName << " : " << requiredInterfaceName;

        ComponentInterfaceProxyServerLoggerError("[NotifyInterfaceConnectionResult] failed to connect - ", buf.str());
        return;
    } else {
        std::stringstream buf;
        buf << "[NotifyInterfaceConnectionResult] succeeded to connect - "
            << (isServerTask ? "Server task: " : "Client task: ")
            << resourceTaskName << " : " << providedInterfaceName << " - "
            << userTaskName << " : " << requiredInterfaceName;

        ComponentInterfaceProxyServerLogger(buf.str());
    }

    //
    // TODO: IMPLEMENT ME
    //
}
*/

//-------------------------------------------------------------------------
//  Send Methods
//-------------------------------------------------------------------------
/*
bool mtsComponentInterfaceProxyServer::SendConnectServerSide(
    TaskManagerClient * taskManagerWithServerTask,
    const std::string & userTaskName,     const std::string & requiredInterfaceName,
    const std::string & resourceTaskName, const std::string & providedInterfaceName)
{
    GetLogger()->trace("TMServer", ">>>>> SEND: ConnectServerSide: " 
            + resourceTaskName + " : " + providedInterfaceName + " - "
            + userTaskName + " : " + requiredInterfaceName);

    return taskManagerWithServerTask->GetClientProxy()->ConnectServerSide(
        userTaskName, requiredInterfaceName, resourceTaskName, providedInterfaceName);
}
*/

//-------------------------------------------------------------------------
//  Definition by mtsComponentInterfaceProxy.ice
//-------------------------------------------------------------------------
mtsComponentInterfaceProxyServer::ComponentInterfaceServerI::ComponentInterfaceServerI(
    const Ice::CommunicatorPtr& communicator, const Ice::LoggerPtr& logger,
    mtsComponentInterfaceProxyServer * componentInterfaceProxyServer)
    : Communicator(communicator),
      SenderThreadPtr(new SenderThread<ComponentInterfaceServerIPtr>(this)),
      Logger(logger),
      Runnable(true),
      ComponentInterfaceProxyServer(componentInterfaceProxyServer)
{
}

void mtsComponentInterfaceProxyServer::ComponentInterfaceServerI::Start()
{
    ComponentInterfaceProxyServer->GetLogger()->trace(
        "mtsComponentInterfaceProxyServer", "Send thread starts");

    SenderThreadPtr->start();
}

// TODO: Remove this
#define _COMMUNICATION_TEST_

void mtsComponentInterfaceProxyServer::ComponentInterfaceServerI::Run()
{
#ifdef _COMMUNICATION_TEST_
    int count = 100;

    while (Runnable) 
    {
        osaSleep(1 * cmn_s);
        std::cout << "Server Proxy [" << (unsigned long) this << "] Running..." << ++count << std::endl;

        std::stringstream ss;
        ss << "Server: ";
        ss << count;

        ComponentInterfaceProxyServer->SendTestMessageFromServerToClient(ss.str());
    }
#else
    while(Runnable) 
    {
        osaSleep(10 * cmn_ms);

        if(!clients.empty())
        {
            ++num;
            for(std::set<mtsTaskManagerProxy::TaskManagerClientPrx>::iterator p 
                = clients.begin(); p != clients.end(); ++p)
            {
                try
                {
                    std::cout << "server sends: " << num << std::endl;
                }
                catch(const IceUtil::Exception& ex)
                {
                    std::cerr << "removing client `" << Communicator->identityToString((*p)->ice_getIdentity()) << "':\n"
                        << ex << std::endl;

                    IceUtil::Monitor<IceUtil::Mutex>::Lock lock(*this);
                    _clients.erase(*p);
                }
            }
        }
    }
#endif
}

void mtsComponentInterfaceProxyServer::ComponentInterfaceServerI::Stop()
{
    if (!ComponentInterfaceProxyServer->IsActiveProxy()) return;

    IceUtil::ThreadPtr callbackSenderThread;
    {
        IceUtil::Monitor<IceUtil::Mutex>::Lock lock(*this);

        Runnable = false;

        notify();

        callbackSenderThread = SenderThreadPtr;
        SenderThreadPtr = 0; // Resolve cyclic dependency.
    }
    callbackSenderThread->getThreadControl().join();
}

//-----------------------------------------------------------------------------
//  Network Event Handlers
//-----------------------------------------------------------------------------
void mtsComponentInterfaceProxyServer::ComponentInterfaceServerI::TestSendMessageFromClientToServer(
    const std::string & str, const ::Ice::Current & current)
{
    Logger->trace("ComponentInterface: Server", "<<<<< RECV: TestSendMessageFromClientToServer");

    ComponentInterfaceProxyServer->TestReceiveMessageFromClientToServer(str);
}

void mtsComponentInterfaceProxyServer::ComponentInterfaceServerI::AddClient(
    const ::Ice::Identity & identity, const ::Ice::Current& current)
{
    IceUtil::Monitor<IceUtil::Mutex>::Lock lock(*this);

    Logger->trace("InterfaceServer", "<<<<< RECV: AddClient: " + Communicator->identityToString(identity));

    ComponentInterfaceClientProxyType clientProxy = 
        ComponentInterfaceClientProxyType::uncheckedCast(current.con->createProxy(identity));
    
    ComponentInterfaceProxyServer->ReceiveAddClient(Communicator->identityToString(identity), clientProxy);
}

void mtsComponentInterfaceProxyServer::ComponentInterfaceServerI::Shutdown(const ::Ice::Current& current)
{
    Logger->trace("InterfaceServer", "<<<<< RECV: Shutdown");

    // Set as true to represent that this connection (session) is going to be closed.
    // After this flag is set, no message is allowed to be sent to a server.
    //ComponentInterfaceProxyServer->ShutdownSession(current);
}

/*
void mtsComponentInterfaceProxyServer::ComponentInterfaceServerI::UpdateTaskManager(
    const mtsTaskManagerProxy::TaskList& localTaskInfo, const ::Ice::Current& current)
{
    Logger->trace("TMServer", "<<<<< RECV: UpdateTaskManager: " + localTaskInfo.taskManagerID);

    //!!!!!!!!!!!!!
    // MJUNG: don't know why this doesn't work. FIXME later.
    //Ice::ImplicitContextPtr a = Communicator->getImplicitContext();
    //std::string s = a->get(ConnectionIDKey);
    // The following line does work.
    //std::string ss = current.ctx.find(ConnectionIDKey)->second;

    TaskManagerServer->ReceiveUpdateTaskManagerClient(
        //Communicator->getImplicitContext()->get(ConnectionIDKey), localTaskInfo);
        current.ctx.find(ConnectionIDKey)->second, localTaskInfo);
}

bool mtsComponentInterfaceProxyServer::ComponentInterfaceServerI::AddProvidedInterface(
    const mtsTaskManagerProxy::ProvidedInterfaceAccessInfo & providedInterfaceAccessInfo,
    const ::Ice::Current & current)
{
    Logger->trace("TMServer", "<<<<< RECV: AddProvidedInterface: " 
        + providedInterfaceAccessInfo.taskName + ", " + providedInterfaceAccessInfo.interfaceName);

    return TaskManagerServer->ReceiveAddProvidedInterface(
        //Communicator->getImplicitContext()->get(ConnectionIDKey), providedInterfaceInfo);
        current.ctx.find(ConnectionIDKey)->second, providedInterfaceAccessInfo);
}

bool mtsComponentInterfaceProxyServer::ComponentInterfaceServerI::AddRequiredInterface(
    const mtsTaskManagerProxy::RequiredInterfaceAccessInfo & requiredInterfaceAccessInfo,
    const ::Ice::Current & current)
{
    Logger->trace("TMServer", "<<<<< RECV: AddRequiredInterface: " 
        + requiredInterfaceAccessInfo.taskName + ", " + requiredInterfaceAccessInfo.interfaceName);

    return TaskManagerServer->ReceiveAddRequiredInterface(
        //Communicator->getImplicitContext()->get(ConnectionIDKey), requiredInterfaceAccessInfo);
        current.ctx.find(ConnectionIDKey)->second, requiredInterfaceAccessInfo);
}

bool mtsComponentInterfaceProxyServer::ComponentInterfaceServerI::IsRegisteredProvidedInterface(
    const ::std::string & taskName, const ::std::string & providedInterfaceName,
    const ::Ice::Current & current) const
{
    Logger->trace("TMServer", "<<<<< RECV: IsRegisteredProvidedInterface: " 
        + taskName + ", " + providedInterfaceName);

    return TaskManagerServer->ReceiveIsRegisteredProvidedInterface(
        //Communicator->getImplicitContext()->get(ConnectionIDKey), taskName, providedInterfaceName);
        current.ctx.find(ConnectionIDKey)->second, taskName, providedInterfaceName);
}

bool mtsComponentInterfaceProxyServer::ComponentInterfaceServerI::GetProvidedInterfaceAccessInfo(
    const ::std::string & taskName, const ::std::string & providedInterfaceName,
    mtsTaskManagerProxy::ProvidedInterfaceAccessInfo & info, const ::Ice::Current & current) const
{
    Logger->trace("TMServer", "<<<<< RECV: GetProvidedInterfaceAccessInfo: " 
        + taskName + ", " + providedInterfaceName);

    return TaskManagerServer->ReceiveGetProvidedInterfaceAccessInfo(
        //Communicator->getImplicitContext()->get(ConnectionIDKey), taskName, providedInterfaceName, info);
        current.ctx.find(ConnectionIDKey)->second, taskName, providedInterfaceName, info);
}

void mtsComponentInterfaceProxyServer::ComponentInterfaceServerI::NotifyInterfaceConnectionResult(
    bool isServerTask, bool isSuccess, 
    const std::string & userTaskName,     const std::string & requiredInterfaceName, 
    const std::string & resourceTaskName, const std::string & providedInterfaceName, 
    const ::Ice::Current & current)
{
    Logger->trace("TMServer", "<<<<< RECV: NotifyInterfaceConnectionResult: " 
        + resourceTaskName + " : " + providedInterfaceName + " - "
        + userTaskName + " : " + requiredInterfaceName);

    TaskManagerServer->ReceiveNotifyInterfaceConnectionResult(
        //Communicator->getImplicitContext()->get(ConnectionIDKey),
        current.ctx.find(ConnectionIDKey)->second,
        isServerTask, isSuccess, 
        userTaskName, requiredInterfaceName, 
        resourceTaskName, providedInterfaceName);
}
*/
