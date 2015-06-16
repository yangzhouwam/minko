/*
Copyright (c) 2014 Aerys

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

#pragma once

#include "minko/component/AbstractLodScheduler.hpp"

namespace minko
{
	namespace component
	{
		class POPGeometryLodScheduler :
			public AbstractLodScheduler
		{
		public:
			typedef std::shared_ptr<POPGeometryLodScheduler>    Ptr;

        private:
            typedef std::shared_ptr<scene::Node>                NodePtr;
        
            typedef std::shared_ptr<SceneManager>               SceneManagerPtr;
            typedef std::shared_ptr<MasterLodScheduler>         MasterLodSchedulerPtr;
            typedef std::shared_ptr<Surface>                    SurfacePtr;

            typedef std::shared_ptr<geometry::Geometry>         GeometryPtr;

            struct SurfaceInfo
            {
                SurfacePtr  surface;

                int         activeLod;

                float       requiredPrecisionLevel;

                SurfaceInfo(SurfacePtr surface) :
                    surface(surface),
                    activeLod(-1),
                    requiredPrecisionLevel(0)
                {
                }
            };

            struct POPGeometryResourceInfo
            {
                ResourceInfo*                               base;

                GeometryPtr                                 geometry;

                int                                         minLod;
                int                                         maxLod;
                int                                         minAvailableLod;
                int                                         maxAvailableLod;
                int                                         fullPrecisionLod;

                std::unordered_map<
                    NodePtr,
                    Signal<data::Store&, ProviderPtr, const data::Provider::PropertyName&>::Slot
                >                                           modelToWorldMatrixChangedSlots;
                std::unordered_map<SurfacePtr, SurfaceInfo> surfaceToSurfaceInfoMap;

                POPGeometryResourceInfo() :
                    base(nullptr),
                    geometry(),
                    minLod(-1),
                    maxLod(-1),
                    minAvailableLod(-1),
                    maxAvailableLod(-1),
                    fullPrecisionLod(-1),
                    modelToWorldMatrixChangedSlots(),
                    surfaceToSurfaceInfoMap()
                {
                }
            };

        private:
            SceneManagerPtr                                             _sceneManager;

            std::unordered_map<std::string, POPGeometryResourceInfo>    _popGeometryResources;

            math::vec3                                                  _eyePosition;
            float                                                       _fov;
            float                                                       _aspectRatio;

            math::vec4                                                  _viewport;

            math::mat4                                                  _worldToScreenMatrix;
            math::mat4                                                  _viewMatrix;

            float                                                       _blendingRange;

        public:
            inline
            static
            Ptr
            create()
            {
                auto instance = Ptr(new POPGeometryLodScheduler());

                return instance;
            }

            void
            blendingRange(float value);

        protected:
            void
            sceneManagerSet(SceneManagerPtr sceneManager);

            void
            masterLodSchedulerSet(MasterLodSchedulerPtr masterLodScheduler);

            void
            surfaceAdded(SurfacePtr surface);

            void
            surfaceRemoved(SurfacePtr surface);

            void
            viewPropertyChanged(const math::mat4&   worldToScreenMatrix,
                                const math::mat4&   viewMatrix,
                                const math::vec3&   eyePosition,
                                float               fov,
                                float               aspectRatio,
                                float               zNear,
                                float               zFar);

            void
            viewportChanged(const math::vec4& viewport);

            void
            maxAvailableLodChanged(ResourceInfo&    resource,
                                   int              maxAvailableLod);

            LodInfo
            lodInfo(ResourceInfo& resource);

        private:
            POPGeometryLodScheduler();

            void
            activeLodChanged(POPGeometryResourceInfo&   resource,
                             SurfaceInfo&               surfaceInfo,
                             int                        previousLod,
                             int                        lod,
                             float                      requiredPrecisionLevel);

            int
            computeRequiredLod(const POPGeometryResourceInfo&   resource,
                               SurfaceInfo&                     surfaceInfo,
                               float&                           requiredPrecisionLevel);

            float
            computeLodPriority(const POPGeometryResourceInfo&  resource,
                               SurfaceInfo&                    surfaceInfo,
                               int                             requiredLod,
                               int                             activeLod);

            bool
            findClosestValidLod(const POPGeometryResourceInfo& resource,
                                int                            lod,
                                ProgressiveOrderedMeshLodInfo& result) const;

            bool
            findClosestLodByPrecisionLevel(const POPGeometryResourceInfo&  resource,
                                           int                             precisionLevel,
                                           ProgressiveOrderedMeshLodInfo&  result) const;

            float
            distanceFromEye(const POPGeometryResourceInfo&  resource,
                            SurfaceInfo&                    surfaceInfo,
                            const math::vec3&               eyePosition);

            void
            requiredPrecisionLevelChanged(const POPGeometryResourceInfo&    resource,
                                          SurfaceInfo&                      surfaceInfo);

            void
            updateBlendingLod(const POPGeometryResourceInfo&    resource,
                              SurfaceInfo&                      surfaceInfo);

            bool
            blendingIsActive(const POPGeometryResourceInfo&    resource,
                             SurfaceInfo&                      surfaceInfo);

            void
            blendingRangeChanged(const POPGeometryResourceInfo&    resource,
                                 SurfaceInfo&                      surfaceInfo,
                                 float                             blendingRange);

            float
            blendingLod(const POPGeometryResourceInfo&    resource,
                        SurfaceInfo&                      surfaceInfo) const;
		};
	}
}
