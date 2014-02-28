/*
Copyright (c) 2013 Aerys

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and
associated documentation files (the "Software"), to deal in the Software without restriction,
including without limitation the rights to use, copy, modify, merge, publish, distribute,
sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or
substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include "minko/Minko.hpp"
#include "minko/MinkoPNG.hpp"
#include "minko/MinkoSDL.hpp"

using namespace minko;
using namespace minko::component;
using namespace minko::math;

static const std::string TEXTURE_FILENAME = "texture/box.png";
static const std::string EFFECT_FILENAME = "effect/Phong.effect";

static const int WINDOW_WIDTH = 800;
static const int WINDOW_HEIGHT = 600;

int main(int argc, char** argv)
{
	auto canvas = Canvas::create("Minko Example - Fog", WINDOW_WIDTH, WINDOW_HEIGHT);

	auto sceneManager = SceneManager::create(canvas->context());
	
	// setup assets
	sceneManager->assets()->defaultOptions()->resizeSmoothly(true);
	sceneManager->assets()->defaultOptions()->generateMipmaps(true);
	sceneManager->assets()
		->registerParser<file::PNGParser>("png")
        ->queue("effect/Sprite.effect")
        ->queue("texture/sprite-pointlight.png")
		->queue(TEXTURE_FILENAME)
		->queue(EFFECT_FILENAME);


	auto _ = sceneManager->assets()->complete()->connect([=](file::AssetLibrary::Ptr assets)
	{
		auto root = scene::Node::create("root")
			->addComponent(sceneManager);

		auto mesh = scene::Node::create("mesh")
			->addComponent(Transform::create(Matrix4x4::create()->appendTranslation(Vector3::create(0.0f, 1.0f, 0.0f))));

		auto camera = scene::Node::create("camera")
			->addComponent(Renderer::create(0x7f7f7fff))
			->addComponent(Transform::create(
			Matrix4x4::create()->lookAt(Vector3::zero(), Vector3::create(0.f, 1.5f, 6.f))
			))
			->addComponent(PerspectiveCamera::create(WINDOW_WIDTH / (float)WINDOW_HEIGHT, (float)PI * 0.25f, .1f, 1000.f));
		root->addChild(camera);
		
        auto material = material::PhongMaterial::create()
            ->diffuseMap(assets->texture(TEXTURE_FILENAME))
            ->fogColor(Vector4::create(1.0f, 1.0f, 1.0f, 1.0f))
            ->fogDensity(0.05f);

        mesh->addComponent(Surface::create(
            geometry::CubeGeometry::create(sceneManager->assets()->context()),
            material,
            assets->effect(EFFECT_FILENAME)));

        auto groundNode = scene::Node::create("ground")
            ->addComponent(Transform::create(Matrix4x4::create()->appendScale(8.0f)->appendRotationX((float) -PI / 2.0f)))
            ->addComponent(Surface::create(
            geometry::QuadGeometry::create(sceneManager->assets()->context()),
            material::PhongMaterial::create()->diffuseColor(0xAA0000FF)->fogColor(Vector4::create(1.0f, 1.0f, 1.0f, 1.0f))
            ->fogDensity(0.05f)
        ,
            assets->effect(EFFECT_FILENAME)));
        root->addChild(groundNode);

        auto ambientLight = scene::Node::create("ambientLight")
            ->addComponent(AmbientLight::create(0.25f));
        ambientLight->component<AmbientLight>()->color(Vector4::create(1.0f, 1.0f, 1.0f, 1.0f));
        root->addChild(ambientLight);

        auto directionalLight = scene::Node::create("directionalLight")
            ->addComponent(DirectionalLight::create()->diffuse(0.8f)->color(0xFFFFFFFF))
            ->addComponent(Transform::create(Matrix4x4::create()->lookAt(Vector3::create(), Vector3::create(3.0f, 2.0f, 3.0f))));
        root->addChild(directionalLight);

        auto pointLightNode = scene::Node::create("pointLight")
            ->addComponent(Transform::create(Matrix4x4::create()->translation(-2.0f, 1.0f, 0.0f)))
            ->addComponent(Surface::create(
            geometry::QuadGeometry::create(assets->context()),
            material::Material::create()
            ->set("diffuseMap", assets->texture("texture/sprite-pointlight.png"))
            ->set("diffuseTint", Vector4::create(1.f, 1.f, 1.f, 1.f)),
            assets->effect("effect/Sprite.effect")
            ))
            ->addComponent(PointLight::create()->diffuse(0.5f));
        root->addChild(pointLightNode);

		std::vector<Matrix4x4::Ptr> keyTransforms;

		keyTransforms.push_back(Matrix4x4::create()->appendTranslation(Vector3::create(0.0f, 0.0f, 0.0f)));
		keyTransforms.push_back(Matrix4x4::create()->copyFrom(keyTransforms[0])->appendTranslation(Vector3::create(0.0f, 0.0f, -15.0f)));
		keyTransforms.push_back(Matrix4x4::create()->copyFrom(keyTransforms[1])->appendTranslation(Vector3::create(0.0f, 0.0f, 15.0f)));

		auto segmentDuration = 1500U;

		auto cubeAnimation = Animation::create(
		{ minko::animation::Matrix4x4Timeline::create("transform.matrix", segmentDuration * 3,
		{ segmentDuration * 0, segmentDuration * 1, segmentDuration * 2 },
		keyTransforms, true) }, true);

		mesh->addComponent(cubeAnimation);

		root->addChild(mesh);

		auto resized = canvas->resized()->connect([&](AbstractCanvas::Ptr canvas, uint w, uint h)
		{
			camera->component<PerspectiveCamera>()->aspectRatio((float)w / (float)h);
		});

		auto enterFrame = canvas->enterFrame()->connect([&](Canvas::Ptr canvas, uint time, uint deltaTime)
		{
			mesh->component<Transform>()->matrix()->prependRotationY(.01f);

			sceneManager->nextFrame();
		});

		canvas->run();
	});

	sceneManager->assets()->load();

	return 0;
}


