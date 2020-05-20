#pragma once

#include "core.hpp"
#include "Shape.hpp"

#include <optional>

namespace idragnev::pbrt {
    class Sphere : public Shape {
    public:
        Sphere(const Transformation& objectToWorld,
            const Transformation& worldToObject,
            const bool reverseOrientation,
            const Float radius,
            const Float zMin,
            const Float zMax,
            const Float phiMax) noexcept;
        
        Bounds3f objectBound() const override;
        
        std::optional<HitRecord> intersect(const Ray& ray, const bool testAlphaTexture) const override;
        bool intersectP(const Ray& ray, const bool testAlphaTexture) const override;

        Float area() const override;
        
    private:
        HitRecord makeHitRecord(const Ray& ray, const Point3f& hitPoint, const EFloat& t, const Float phi) const;

        std::optional<QuadraticRoots> findIntersectionParams(const Ray& ray, const Vector3f& oErr, const Vector3f& dErr) const;
        
        Point3f computeHitPoint(const Ray& ray, const EFloat& t) const;
        
        bool liesInZClippedArea(const Point3f& hitPoint) const noexcept;

        static Float computePhi(const Point3f& hitPoint);

    private:
        Float radius;
        Float zMin;
        Float zMax;
        Float thetaMin;
        Float thetaMax;
        Float phiMax;
    };
} //namespace idragnev::pbrt