#pragma once

#include "pbrt/core/core.hpp"
#include "pbrt/core/Shape.hpp"

namespace idragnev::pbrt::shapes {
    class Cone : public Shape
    {
    public:
        Cone(const Transformation& objectToWorld,
             const Transformation& worldToObject,
             const bool reverseOrientation,
             const Float height,
             const Float radius,
             const Float phiMax) noexcept;

        Bounds3f objectBound() const override;

        Optional<HitRecord>
        intersect(const Ray& ray, const bool testAlphaTexture) const override;

        bool intersectP(const Ray& ray,
                        const bool testAlphaTexture) const override;

        Float area() const override;

    private:
        template <typename R, typename S, typename F>
        R intersectImpl(const Ray& ray, F failure, S success) const;

        HitRecord makeHitRecord(const RayWithErrorBound& ray,
                                const Point3f& hitPoint,
                                const EFloat& t,
                                const Float phi) const;

        Optional<QuadraticRoots>
        findIntersectionParams(const Ray& ray,
                               const Vector3f& oErr,
                               const Vector3f& dErr) const;

        static Float computePhi(const Point3f& hitPoint);

    private:
        Float radius;
        Float height;
        Float phiMax;
    };
} // namespace idragnev::pbrt::shapes