#include <testsuite/Tests.h>
#include <ege/debug/Dump.h>
#include <ege/egeNetwork/EGEClient.h>
#include <ege/egeNetwork/EGEPacket.h>
#include <ege/egeNetwork/EGEServer.h>
#include <ege/egeNetwork/ServerNetworkController.h>
#include <ege/event/SystemEventHandler.h>
#include <ege/event/SystemWindow.h>
#include <ege/gui/GUIGameLoop.h>
#include <ege/gui/GUIScreen.h>
#include <ege/scene/Scene2D.h>
#include <ege/scene/SceneWidget.h>
#include <ege/scene/TexturedRenderer2D.h>
#include <ege/util/ObjectInt.h>
#include <ege/util/ObjectMap.h>
#include <ege/util/ObjectString.h>
#include <functional>
#include <iomanip>
#include <iostream>
#include <memory>

TESTCASE(converter)
{
    std::shared_ptr<EGE::ObjectMap> map = make<EGE::ObjectMap>();
    map->addObject("packetTestInt", make<EGE::ObjectInt>(62452));
    map->addObject("String", make<EGE::ObjectString>("test"));
    map->addObject("IntAsString", make<EGE::ObjectString>("543"));
    std::shared_ptr<EGE::ObjectMap> subMap = make<EGE::ObjectMap>();
    subMap->addObject("MapTest", make<EGE::ObjectString>("YYYTEST555??"));
    map->addObject("SubMapTest", subMap);

    std::cerr << "Object: " << map->toString() << std::endl;

    EGE::EGEPacket packet(EGE::EGEPacket::Type::_Data, map);
    sf::Packet sfPacket = packet.toSFMLPacket();

    std::cerr << "Hex dump: " << std::endl;
    EGE::hexDump(sfPacket.getData(), sfPacket.getDataSize(), EGE::HexDumpSettings{8});

    EGE::EGEPacket packet2(sfPacket);
    std::cerr << "Parsed object: " << packet2.getArgs()->toString() << std::endl;
    std::cerr << std::dec;
}

class MyObject : public EGE::SceneObject2D
{
public:
    MyObject(std::shared_ptr<EGE::Scene> owner, bool playerControlled = false)
    : EGE::SceneObject2D(owner, "test-egeNetwork:MyObject")
    {
        // only server side!
        if(!playerControlled && owner->isHeadless())
        {
            auto timer = make<EGE::Timer>(this, EGE::Timer::Mode::Limited, EGE::Time(10.0, EGE::Time::Unit::Seconds));
            timer->setCallback([this](std::string, EGE::Timer*) {
                                    std::cerr << "[[[ object " << getObjectId() << " was removed ]]]" << std::endl;
                                    m_dead = true;
                               });

            addTimer("timer", timer);
            m_motion = sf::Vector2f(rand() % 5 - 2, rand() % 5 - 2);
        }
        m_rotation = rand() % 360;
        m_scale = sf::Vector2f(rand() % 2 + 1, rand() % 2 + 1);
    }

    virtual void render(sf::RenderTarget& target) const
    {
        EGE::SceneObject2D::render(target);
        sf::RectangleShape rs(sf::Vector2f(10.f, 10.f));
        rs.setPosition(getPosition() - sf::Vector2f(5.f, 5.f));
        rs.setRotation(m_rotation);
        rs.setScale(m_scale);
        rs.setOrigin(m_origin);
        target.draw(rs);
    }

    virtual void move(bool moving, int dir)
    {
        if(moving)
        {
            switch(dir)
            {
                case 0: m_motion.x = -5.f; break; // a
                case 1: m_motion.y = 5.f; break; // s
                case 2: m_motion.x = 5.f; break; // d
                case 3: m_motion.y = -5.f; break; // w
            }
        }
        else
        {
            switch(dir)
            {
                case 0: if(m_motion.x == -5.f) m_motion.x = 0.f; break; // a
                case 1: if(m_motion.y == 5.f) m_motion.y = 0.f; break; // s
                case 2: if(m_motion.x == 5.f) m_motion.x = 0.f; break; // d
                case 3: if(m_motion.y == -5.f) m_motion.y = 0.f; break; // w
            }
        }
        setMainChanged();
    }
};

class MyObjectServerController : public EGE::ServerNetworkController
{
public:
    MyObjectServerController(std::shared_ptr<EGE::SceneObject> object, EGE::EGEServer* server)
    : EGE::ServerNetworkController(object, server) {}

    virtual void handleRequest(const EGE::ControlObject& data) override
    {
        std::string type = data.getType();
        MyObject* object = (MyObject*)getObject().get();
        bool moving = data.getArgs()->getObject("moving").lock()->asInt();
        int dir = data.getArgs()->getObject("dir").lock()->asInt();

        if(type == "move")
            object->move(moving, dir);
    }
};

class MyObjectClientController : public EGE::ClientNetworkController
{
public:
    MyObjectClientController(std::shared_ptr<EGE::SceneObject> object, EGE::EGEClient* client)
    : EGE::ClientNetworkController(object, client) {}

    virtual void handleRequest(const EGE::ControlObject& data) override
    {
        std::string type = data.getType();
        MyObject* object = (MyObject*)getObject().get();
        bool moving = data.getArgs()->getObject("moving").lock()->asInt();
        int dir = data.getArgs()->getObject("dir").lock()->asInt();

        if(type == "move")
            object->move(moving, dir);
    }
};

class MyServer : public EGE::EGEServer
{
public:
    MyServer()
    : EGE::EGEServer(rand() % 63536 + 2000) {}

    virtual std::shared_ptr<EGE::ServerNetworkController> makeController(std::shared_ptr<EGE::SceneObject> object)
    {
        if(object->getId() == "test-egeNetwork:MyObject")
        {
            // MyObject is controlled by MyObjectServerController
            return make<MyObjectServerController>(object, this);
        }

        // object cannot be controlled
        return nullptr;
    }

    virtual EGE::EventResult onLogin(EGE::EGEClientConnection* client, std::shared_ptr<EGE::ObjectMap> data)
    {
        // Synchronize client with server.
        EGE::EGEServer::onLogin(client, data);

        // Spawn SceneObject that will be controlled by this client.
        auto sceneObject = make<MyObject>(getScene(), true);
        std::cerr << "Adding Object to Scene" << std::endl;
        getScene()->addObject(sceneObject);

        // Set SceneObject to be controlled by this Client.
        setDefaultController(client, sceneObject);

        return EGE::EventResult::Success;
    }
};

class MyClient : public EGE::EGEClient
{
public:
    MyClient(unsigned short port)
    : EGE::EGEClient(sf::IpAddress::LocalHost, port) {}

    virtual std::shared_ptr<EGE::ClientNetworkController> makeController(std::shared_ptr<EGE::SceneObject> object)
    {
        if(object->getId() == "test-egeNetwork:MyObject")
        {
            // MyObject is controlled by MyObjectClientController
            return make<MyObjectClientController>(object, this);
        }

        // object cannot be controlled
        return nullptr;
    }

    void move(bool moving, int dir)
    {
        std::shared_ptr<EGE::ObjectMap> _map = make<EGE::ObjectMap>();
        _map->addObject("moving", make<EGE::ObjectInt>(moving));
        _map->addObject("dir", make<EGE::ObjectInt>(dir));
        requestControl(nullptr, EGE::ControlObject("move", _map));
    }
};

TESTCASE(server)
{
    srand(time(nullptr));

    auto serverThread = []() {
        MyServer server;

        server.setMinimalTickTime(EGE::Time(1 / 60.0, EGE::Time::Unit::Seconds)); //60 tps

        auto scene = make<EGE::Scene2D>(nullptr);

        auto timer = make<EGE::Timer>(&server, EGE::Timer::Mode::Infinite, EGE::Time(2.0, EGE::Time::Unit::Seconds));
        timer->setCallback([scene](std::string, EGE::Timer*) {
                                auto object = make<MyObject>(scene);
                                object->setPosition(sf::Vector2f(rand() % 50 - 25, rand() % 50 - 25));
                                scene->addObject(object);
                           });

        server.addTimer("timer", timer);

        server.setScene(scene);
        return server.run();
    };
    sf::Thread thread1(serverThread);
    thread1.launch();
}

class MySystemEventHandler : public EGE::DefaultSystemEventHandler
{
public:
    MySystemEventHandler(std::weak_ptr<EGE::SFMLSystemWindow> window, std::shared_ptr<MyClient> client)
    : EGE::DefaultSystemEventHandler(window), m_client(client) {}

    void onKeyPress(sf::Event::KeyEvent& event)
    {
        switch(event.code)
        {
        case sf::Keyboard::A: m_client->move(true, 0);
            break;
        case sf::Keyboard::S: m_client->move(true, 1);
            break;
        case sf::Keyboard::D: m_client->move(true, 2);
            break;
        case sf::Keyboard::W: m_client->move(true, 3);
            break;
        default:
            break;
        }
    }

    void onKeyRelease(sf::Event::KeyEvent& event)
    {
        switch(event.code)
        {
        case sf::Keyboard::A: m_client->move(false, 0);
            break;
        case sf::Keyboard::S: m_client->move(false, 1);
            break;
        case sf::Keyboard::D: m_client->move(false, 2);
            break;
        case sf::Keyboard::W: m_client->move(false, 3);
            break;
        default:
            break;
        }
    }

    // It's done by GUIScreen.
    virtual void onClose() {}

private:
    std::shared_ptr<MyClient> m_client;
};

class MyGameLoop : public EGE::GUIGameLoop
{
    std::shared_ptr<MyClient> m_client;
    std::shared_ptr<EGE::CameraObject2D> m_camera;
    int m_port;

public:
    MyGameLoop(int port)
    : m_port(port) {}

    EGE::EventResult onLoad()
    {
        // Call base onLoad to create profiler, load resources etc.
        if(EGE::GUIGameLoop::onLoad() == EGE::EventResult::Failure)
            return EGE::EventResult::Failure;

        // Create GUI and assign it to client.
        auto gui = make<EGE::GUIScreen>(this);
        setCurrentGUIScreen(gui);

        // Create client - define server IP and port.
        m_client = make<MyClient>(m_port);

        // Create scene and assign it to client.
        auto scene = make<EGE::Scene2D>(this);
        m_client->setScene(scene);

        // Create SceneWidget to be displayed in the window.
        gui->addWidget(make<EGE::SceneWidget>(gui.get(), scene));

        // Set initialize handler for Client. It will be called before connecting.
        m_client->setInitializeHandler([](EGE::EGEGame::GPOM* gpom) {
                                gpom->sceneObjectCreators.add("test-egeNetwork:MyObject", new std::function(
                                                                [](std::shared_ptr<EGE::Scene> scene)->std::shared_ptr<EGE::SceneObject> {
                                                                    return make<MyObject>(scene);
                                                                }));
                                return true;
                            });

        // Set exit handler for Client. It will be called when client is disconnected.
        m_client->setExitHandler([this](int retVal) {
                                    std::cerr << "CLIENT CLOSED, R=" << retVal << std::endl;
                                    exit(retVal);
                                 });

        // Add keybind handler. It will pass keyboard events to Controller.
        addEventHandler(EGE::SystemEvent::getTypeStatic(), make<MySystemEventHandler>(getWindow(), m_client));

        // Initialize Camera.
        m_camera = make<EGE::CameraObject2D>(scene);
        m_camera->setScalingMode(EGE::ScalingMode::Centered);
        scene->setCamera(m_camera);

        // Set our Client as sub-loop. It will automatically connect to server now.
        setSubLoop(m_client);

        return EGE::EventResult::Success;
    }

    void onTick(long long tick)
    {
        EGE::GUIGameLoop::onTick(tick);
        // Get currently controlled object
        auto controller = m_client->getDefaultController();
        if(controller)
        {
            EGE::SceneObject2D* obj = (EGE::SceneObject2D*)controller->getObject().get();
            if(obj)
                m_camera->setPosition(obj->getPosition());
        }
    }
};

TESTCASE(client)
{
    int PORT = 0;

    // Ask user for port.
    // Yes, there is no error handling...
    std::cout << "Enter port: ";
    std::cin >> PORT;

    // Create GameLoop and window.
    MyGameLoop loop(PORT);
    loop.setWindow(make<EGE::SFMLSystemWindow>(sf::VideoMode(300, 300), "EGE Protocol Test"));
    loop.getWindow().lock()->setKeyRepeatEnabled(false);
    loop.setMinimalTickTime(EGE::Time(1 / 60.0, EGE::Time::Unit::Seconds)); //60 fps

    // Run main loop.
    loop.run();

    // Here doing ANYTHING is NOT SAFE!!! (because of destructors)
}

RUN_TESTS(egeNetwork)
