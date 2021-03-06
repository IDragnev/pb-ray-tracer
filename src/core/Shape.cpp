#include "pbrt/core/Shape.hpp"
#include "pbrt/core/transformations/Transformation.hpp"
#include "pbrt/core/geometry/Bounds3.hpp"

namespace idragnev::pbrt {
    Shape::Shape(const Transformation& objectToWorld,
                 const Transformation& worldToObject,
                 const bool reverseOrientation) noexcept
        : objectToWorldTransform(&objectToWorld)
        , worldToObjectTransform(&worldToObject)
        , reverseOrientation(reverseOrientation)
        , transformSwapsHandedness(objectToWorld.swapsHandedness()) {}

    Bounds3f Shape::worldBound() const {
        return (*objectToWorldTransform)(objectBound());
    }

    bool Shape::intersectP(const Ray& ray, const bool testAlphaTexture) const {
        return intersect(ray, testAlphaTexture).has_value();
    }
} // namespace idragnev::pbrt