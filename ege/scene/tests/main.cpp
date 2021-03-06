#include <testsuite/Tests.h>
#include <ege/debug/Logger.h>
#include <ege/gfx/Renderer.h>
#include <ege/gui/Animation.h>
#include <ege/gui/AnimationEasingFunctions.h>
#include <ege/gui/GUIGameLoop.h>
#include <ege/gui/Label.h>
#include <ege/scene/ParticleSystem2D.h>
#include <ege/scene/Scene.h>
#include <ege/scene/SceneLoader.h>
#include <ege/scene/SceneWidget.h>
#include <ege/scene/Plain2DCamera.h>
#include <ege/tilemap/ChunkedTileMap2D.h>
#include <ege/tilemap/FixedTileMap2D.h>
#include <ege/util/system.h>

// my object definition
class MyObject : public EGE::SceneObject
{
    EGE::SharedPtr<sf::Font> m_font;

public:
    EGE_SCENEOBJECT("MyObject");

    // Objects registered in Scene Object Creator must have its "empty" state!
    MyObject(EGE::Scene& owner)
    : EGE::SceneObject(owner) {}

    virtual void onInit() override
    {
        // make object animated (it can be done now by flyTo())
        auto anim = make<EGE::Vec2Animation>(*this, 1.0, EGE::Timer::Mode::Infinite);
        anim->addKeyframe(0.0, getPosition().toVec2d() - EGE::Vec2d(20.0, 20.0));
        anim->addKeyframe(0.5, getPosition().toVec2d() + EGE::Vec2d(20.0, 20.0));
        anim->addKeyframe(1.0, getPosition().toVec2d() - EGE::Vec2d(20.0, 20.0));
        anim->setEasingFunction(EGE::AnimationEasingFunctions::easeInOutQuad);
        addAnimation<EGE::Vec2d>(anim, [this](EGE::Vec2Animation&, EGE::Vec2d val) {
            setPosition(val);
        });
    }

    void updateGeometry(EGE::Renderer&) override
    {
        if(!m_font)
        {
            m_font = m_owner.getLoop()->getResourceManager()->getDefaultFont();
            ASSERT(m_font);
        }
    }

    void render(EGE::Renderer& renderer) const override
    {
        EGE::Renderer::TextWithBackgroundSettings settings;
        settings.font_size = 18;
        settings.text_align = EGE::Renderer::TextAlign::Center;
        settings.color = EGE::Colors::white;
        settings.background_color = EGE::Colors::transparent;
        renderer.renderTextWithBackground(getPosition().x, getPosition().y, *m_font, "MyObject: " + getName(), settings);
    }

    EGE::SharedPtr<EGE::ObjectMap> serializeMain() const override
    {
        auto object = EGE::SceneObject::serializeMain();
        object->addObject("ip", EGE::Serializers::fromVector2(m_initialPosition));
        return object;
    }

    void setDead() { m_dead = true; }

private:
    EGE::Vec2d m_initialPosition;
};

class MyBackground : public EGE::SceneObject
{
public:
    EGE_SCENEOBJECT("MyBackground");

    MyBackground(EGE::Scene& owner)
    : EGE::SceneObject(owner)
    {
        auto anim = make<EGE::RGBAnimation>(*this, 5.0, EGE::Timer::Mode::Infinite);
        anim->addKeyframe(0.0, EGE::Colors::red);
        anim->addKeyframe(0.25, EGE::Colors::cyan);
        anim->addKeyframe(0.5, EGE::Colors::magenta);
        anim->addKeyframe(0.75, EGE::Colors::white);
        anim->addKeyframe(1.0, EGE::Colors::red);
        anim->setEasingFunction(EGE::AnimationEasingFunctions::easeInOutQuad);
        addAnimation<EGE::ColorRGBA>(anim, [this](EGE::RGBAnimation&, EGE::ColorRGBA val) {
            m_color = val;
        });
    }
    void render(EGE::Renderer& renderer) const override
    {
        // add our 'test' shader
        EGE::RenderStates myStates = renderer.getStates();
        /*auto shader = getOwner().getLoop()->getResourceManager()->getShader("test");
        double disturb1 = (float)std::sin(getOwner().getLoop()->time(EGE::Time::Unit::Seconds) * 5.14);
        double disturb2 = (float)std::sin(getOwner().getLoop()->time(EGE::Time::Unit::Seconds) * 1.14);
        shader->setUniform("disturb1", (float)disturb1);
        shader->setUniform("disturb2", (float)disturb2);
        myStates.sfStates().shader = shader.get();*/

        sf::VertexArray varr(sf::Quads, 4);
        varr.append(sf::Vertex(sf::Vector2f(getPosition().x-100.f, getPosition().y-100.f), sf::Color(m_color.r * 255, m_color.g * 255, m_color.b * 255, m_color.a * 255)));
        varr.append(sf::Vertex(sf::Vector2f(getPosition().x+100.f, getPosition().y-100.f), sf::Color::Green));
        varr.append(sf::Vertex(sf::Vector2f(getPosition().x+100.f, getPosition().y+100.f), sf::Color::Blue));
        varr.append(sf::Vertex(sf::Vector2f(getPosition().x-100.f, getPosition().y+100.f), sf::Color::Yellow));
        renderer.getTarget().draw(varr, myStates.sfStates());
    }

    EGE::ColorRGBA m_color;
};

// resource manager
class MyResourceManager : public EGE::ResourceManager
{
public:
    // called on game start!
    virtual bool reload() override
    {
        // define default font to be used by EGE Widgets (we are using them in MyObject::render())
        // it will be automatically loaded by engine.
        bool success = setDefaultFont("font.ttf");
        success &= (bool)loadTextureFromFile("texture.png").get();
        success &= (bool)loadTextureFromFile("texturedPart.png").get();
        success &= (bool)loadShaderFromFile("test", "test.vert", "test.frag").get();

        // return true if all resources successfully loaded
        return success;
    }
};

TESTCASE(serializer)
{
    // create loop
    EGE::GUIGameLoop gameLoop;
    gameLoop.openWindow(sf::VideoMode(300, 300), "EGE Scene Serializer Test");

    // limit window framerate to 10
    gameLoop.getWindow().setFramerateLimit(60);

    // create main GUI
    auto gui = make<EGE::GUIScreen>(gameLoop);

    // create scene
    auto scene = make<EGE::Scene>(&gameLoop);

    // add object types
    auto& registry = scene->getRegistry();
    registry.addType<MyBackground>();

    // create some object
    auto myObject = scene->createObject<MyBackground>(nullptr);
    myObject->setName("My Test");
    myObject->setPosition(EGE::Vec2d(0.f, 0.f));

    // serialize object
    auto data = myObject->serialize();
    std::cerr << data->toString() << std::endl;

    // deserialize object and add result
    auto myObject2 = scene->addNewObject<MyBackground>(data);
    myObject2->setName("My Object 5555");
    myObject2->setPosition(EGE::Vec2d(-100.f, -100.f));

    // assign scene to GUI
    gui->addWidget(make<EGE::SceneWidget>(*gui, scene));

    // assign an instance of MyResourceManager to game loop
    gameLoop.setResourceManager(make<MyResourceManager>());

    // assign GUI to loop
    gameLoop.setCurrentGUIScreen(gui);

    return gameLoop.run();
}

struct ParticleData : public EGE::ParticleSystem2D::UserData
{
    float motionx = 0.f, motiony = 0.5f;
    float color = 1.f;
    float ccp = 200.f;
};

class MyScene : public EGE::Scene
{
public:
    explicit MyScene(EGE::GUIGameLoop* loop)
    : EGE::Scene(loop) {}

    virtual void onUpdate(long long tickCounter)
    {
        // Update another objects
        EGE::Scene::onUpdate(tickCounter);

        // Update wind
        wind = std::sin((tickCounter + rand() % 100 / 100.f) / 100.f) * 100.f;
    }

    float wind = 0.f;
};

TESTCASE(particleSystem)
{
    // create loop
    EGE::GUIGameLoop loop;
    loop.openWindow(sf::VideoMode(600, 600), "Particle System");
    loop.setMinimalTickTime(EGE::Time(1 / 60.0, EGE::Time::Unit::Seconds));

    // create scene
    EGE::SharedPtr<MyScene> scene = make<MyScene>(&loop);

    // add wind speed variable
    //float wind = 0.f;

    // create particle system
    EGE::SharedPtr<EGE::ParticleSystem2D> particleSystem = scene->addNewObject<EGE::ParticleSystem2D>();
    particleSystem->setSpawnRect({10.f, 10.f, 580.f, 1.f});
    particleSystem->setSpawnChance(50.0);
    particleSystem->setParticleLifeTime(400);
    particleSystem->setParticleUpdater([](EGE::ParticleSystem2D::Particle& particle) {
        ParticleData* myData = (ParticleData*)particle.userData.get();
        float meltFactor = (1.f - myData->color / 1.1f);

        // Gravity
        myData->motiony += 0.05f * meltFactor;
        particle.position.y += myData->motiony;
        if(particle.position.y > myData->ccp && myData->color > 0.f)
        {
            myData->color -= (rand() % 100) / 2500.f + 0.005f;
            if(myData->color < 0.f)
                myData->color = 0.f;
        }

        // Wind
        float wind = ((MyScene&)particle.system.getOwner()).wind;
        myData->motionx = wind / (particle.position.y + 0.5f) * meltFactor;
        particle.position.x += myData->motionx;
    });

    particleSystem->setParticleRenderer([](const std::list<EGE::ParticleSystem2D::Particle>& particles, EGE::Renderer& renderer) {
        ege_log.debug() << "Particles: " << particles.size();

        // Generate vertexes.
        std::vector<EGE::Vertex> vertexes;
        for(const EGE::ParticleSystem2D::Particle& particle: particles)
        {
            ParticleData* myData = (ParticleData*)particle.userData.get();
            float clf = (myData->color + 4.f) / 5.2f;
            sf::Color color(clf * 255, clf * 255, 255);
            vertexes.push_back(EGE::Vertex::make(EGE::Vec3d(particle.position.x, particle.position.y, 0.0), color));
            vertexes.push_back(EGE::Vertex::make(EGE::Vec3d(particle.position.x + myData->motionx, particle.position.y + myData->motiony, 0.0), color));
        }

        // Actually render them.
        renderer.renderPrimitives(vertexes, sf::Lines);
    });
    particleSystem->setParticleOnSpawn([](EGE::ParticleSystem2D::Particle& particle) {
        // Create user data instance.
        particle.userData = std::make_unique<ParticleData>();
        ParticleData* myData = (ParticleData*)particle.userData.get();

        // Randomize "melt" position.
        myData->ccp = rand() % 100 + 150.f;
    });

    // add camera
    auto cam = scene->addNewObject<EGE::Plain2DCamera>();
    cam->setScalingMode(EGE::ScalingMode::None);
    scene->setCamera(cam);

    // create GUI
    EGE::SharedPtr<EGE::GUIScreen> gui = make<EGE::GUIScreen>(loop);
    gui->addWidget(make<EGE::SceneWidget>(*gui, scene));

    // assign GUI to loop
    loop.setCurrentGUIScreen(gui);

    // run game
    return loop.run();
}

class MyResourceManager2 : public EGE::ResourceManager
{
public:
    virtual bool reload() override
    {
        bool success = true;
        success &= (bool)loadTextureFromFile("atlas.png");
        return success;
    }
};

TESTCASE(sceneLoader)
{
    // Create loop (it's needed for Scene)
    EGE::GUIGameLoop loop;

    // Load some scene
    auto scene = make<EGE::Scene>(&loop);

    EGE::SceneObjectRegistry& registry = scene->getRegistry();
    registry.addType<MyObject>();
    registry.addType<MyBackground>();

    EGE::SceneLoader loader(*scene);
    if(!loader.loadSceneAndSave("test.json", "scenes/test.json"))
    {
        ege_log.error() << "Failed to load scene!";
        return 1;
    }

    // Add camera
    auto camera = scene->addNewObject<EGE::Plain2DCamera>(nullptr);
    camera->setPosition({0, 0});
    camera->setScalingMode(EGE::ScalingMode::Centered);
    scene->setCamera(camera);

    // Change something in loaded static objects
    {
        auto so1 = scene->getObjectByName("2");
        so1->setName("S__O1");
        auto so2 = scene->getObjectByName("3");
        so2->setName("S__O2");
    }

    // Assign Resource Manager
    loop.setResourceManager(make<MyResourceManager>());

    // Open some window to see results.
    loop.openWindow(sf::VideoMode(600, 600), "SceneLoader");
    auto gui = make<EGE::GUIScreen>(loop);
    gui->addWidget(make<EGE::SceneWidget>(*gui, scene));
    loop.setCurrentGUIScreen(gui);

    if(loop.run() != 0)
        return 2;

    // Try save scene
    EGE::System::createDirectory("saves");
    if(!loader.saveScene("test.json"))
        return 3;

    return 0;
}

class SimpleRectangleObject : public EGE::SceneObject
{
public:
    EGE_SCENEOBJECT("SimpleRectangleObject");

    SimpleRectangleObject(EGE::Scene& scene)
    : EGE::SceneObject(scene)
    {
        auto anim = make<EGE::Vec2Animation>(*this, 1.0, EGE::Timer::Mode::Infinite);
        anim->addKeyframe(0.0, m_initialPosition - EGE::Vec2d(10.0, 0));
        anim->addKeyframe(0.5, m_initialPosition + EGE::Vec2d(10.0, 0));
        anim->addKeyframe(1.0, m_initialPosition - EGE::Vec2d(10.0, 0));
        anim->setEasingFunction(EGE::AnimationEasingFunctions::easeInOutQuad);
        addAnimation<EGE::Vec2d>(anim, [this](EGE::Vec2Animation&, EGE::Vec2d val) {
            setPosition(val);
        });
    }

    virtual void render(EGE::Renderer& renderer) const
    {
        sf::RectangleShape rs(sf::Vector2f(20, 20));
        rs.setOrigin(10, 10);
        auto pos = getPosition();
        rs.setPosition({(float)pos.x, (float)pos.y});
        rs.setRotation(getRotation());
        renderer.getTarget().draw(rs);
    }

    virtual void onUpdate(long long tickCounter)
    {
        EGE::SceneObject::onUpdate(tickCounter);
        if(!m_parent)
            setRotation(getRotation() + 0.01);
    }

    EGE::Vec2d m_initialPosition;
};

TESTCASE(parenting)
{
    // Setup loop and load scene
    EGE::GUIGameLoop loop;
    auto scene = make<EGE::Scene>(&loop);

    // Load other objects
    EGE::SceneObjectRegistry& registry = scene->getRegistry();
    if(!registry.loadFromFile("objects/registry.json"))
        return 3;

    registry.addType<MyObject>();
    registry.addType<MyBackground>();
    registry.addType<SimpleRectangleObject>();

    if(!scene->loadFromFile("parenting.json", "scenes/parenting.json"))
        return 1;

    // Add camera
    {
        auto camera = scene->addNewObject<EGE::Plain2DCamera>();
        camera->setPosition({0, 0});
        camera->setScalingMode(EGE::ScalingMode::Centered);
        scene->setCamera(camera);
    }

    // Assign ResourceManager & open window & GUI
    loop.setResourceManager(make<MyResourceManager>());
    loop.openWindow(sf::VideoMode(600, 600), "Parenting");
    auto gui = make<EGE::GUIScreen>(loop);
    gui->addWidget(make<EGE::SceneWidget>(*gui, scene));
    loop.setCurrentGUIScreen(gui);

    // Run game
    loop.run();

    // Save scene
    if(!scene->saveToFile("parenting.json"))
        return 2;
    return 0;
}

RUN_TESTS(scene);
