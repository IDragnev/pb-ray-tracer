#pragma once

#include "pbrt/core/math/Vector3.hpp"
#include "pbrt/core/math/Interval.hpp"
#include "Ray.hpp"

namespace idragnev::pbrt {
    template <typename T>
    Bounds3<T>::Bounds3() {
        constexpr auto minNum = std::numeric_limits<T>::lowest();
        constexpr auto maxNum = std::numeric_limits<T>::max();
        min = {maxNum, maxNum, maxNum};
        max = {minNum, minNum, minNum};
    }

    template <typename T>
    inline Bounds3<T>::Bounds3(const math::Point3<T>& p) : min(p)
                                                         , max(p) {}

    template <typename T>
    Bounds3<T>::Bounds3(const math::Point3<T>& p1, const math::Point3<T>& p2)
        : min{math::min(p1, p2)}
        , max{math::max(p1, p2)} {}

    template <typename T>
    template <typename U>
    inline Bounds3<T>::operator Bounds3<U>() const {
        static_assert(std::is_convertible_v<T, U>,
                      "Cannot convert the underlying type");
        return Bounds3<U>{math::Point3<U>{min}, math::Point3<U>{max}};
    }

    template <typename T>
    Optional<Intervalf> Bounds3<T>::intersectP(const Ray& ray) const noexcept {
        Float t0 = 0.f;
        Float t1 = ray.tMax;

        for (std::size_t i = 0; i < 3; ++i) {
            const Float invRayDir = 1 / ray.d[i];
            Float tNear = (min[i] - ray.o[i]) * invRayDir;
            Float tFar = (max[i] - ray.o[i]) * invRayDir;

            if (tNear > tFar) {
                std::swap(tNear, tFar);
            }

            tFar *= 1.f + 2.f * gamma(3);

            t0 = tNear > t0 ? tNear : t0;
            t1 = tFar < t1 ? tFar : t1;

            if (t0 > t1) {
                return pbrt::nullopt;
            }
        }

        return pbrt::make_optional(Intervalf{t0, t1});
    }

    template <typename T>
    bool Bounds3<T>::intersectP(const Ray& ray,
                                const Vector3f& invDir,
                                const std::size_t dirIsNeg[3]) const noexcept {
        assert(std::all_of(dirIsNeg, dirIsNeg + 3, [](const auto n) {
            return n == 0 || n == 1;
        }));

        const auto& bounds = *this;
        constexpr auto k = 1.f + 2.f * gamma(3);

        const Float txMin = (bounds[dirIsNeg[0]].x - ray.o.x) * invDir.x;
        const Float txMax =
            (bounds[1 - dirIsNeg[0]].x - ray.o.x) * invDir.x * k;
        const Float tyMin = (bounds[dirIsNeg[1]].y - ray.o.y) * invDir.y;
        const Float tyMax =
            (bounds[1 - dirIsNeg[1]].y - ray.o.y) * invDir.y * k;

        if (txMin > tyMax || tyMin > txMax) {
            return false;
        }

        Float tMin = txMin > tyMin ? txMin : tyMin;
        Float tMax = txMax < tyMax ? txMax : tyMax;

        const Float tzMin = (bounds[dirIsNeg[2]].z - ray.o.z) * invDir.z;
        const Float tzMax =
            (bounds[1 - dirIsNeg[2]].z - ray.o.z) * invDir.z * k;

        if (tMin > tzMax || tzMin > tMax) {
            return false;
        }

        if (tzMin > tMin) {
            tMin = tzMin;
        }
        if (tzMax < tMax) {
            tMax = tzMax;
        }

        return (tMin < ray.tMax) && (tMax > 0.f);
    }

    template <typename T>
    inline math::Vector3<T> Bounds3<T>::diagonal() const {
        return max - min;
    }

    template <typename T>
    inline T Bounds3<T>::surfaceArea() const {
        const auto d = diagonal();
        return 2 * (d.x * d.y + d.x * d.z + d.y * d.z);
    }

    template <typename T>
    inline T Bounds3<T>::volume() const {
        const auto d = diagonal();
        return d.x * d.y * d.z;
    }

    template <typename T>
    std::size_t Bounds3<T>::maximumExtent() const {
        const auto d = diagonal();
        if (d.x > d.y && d.x > d.z) {
            return 0;
        }
        else if (d.y > d.z) {
            return 1;
        }
        else {
            return 2;
        }
    }

    template <typename T>
    inline math::Vector3<T> Bounds3<T>::offset(const math::Point3<T>& p) const {
        auto o = p - min;
        if (max.x > min.x) {
            o.x /= max.x - min.x;
        }
        if (max.y > min.y) {
            o.y /= max.y - min.y;
        }
        if (max.z > min.z) {
            o.z /= max.z - min.z;
        }

        return o;
    }

    template <typename T>
    inline math::Point3<T>& Bounds3<T>::operator[](std::size_t i) {
        return const_cast<math::Point3<T>&>(
            static_cast<const Bounds3&>(*this)[i]);
    }

    template <typename T>
    inline const math::Point3<T>& Bounds3<T>::operator[](std::size_t i) const {
        assert(i < 2);
        return (i == 0) ? min : max;
    }

    template <typename T>
    BoundingSphere3<T> Bounds3<T>::boundingSphere() const {
        const auto center = (min + max) / 2;
        BoundingSphere3<T> result;
        result.center = center;
        result.radius = inside(center, *this) ? distance(center, max) : 0.0;
        return result;
    }

    template <typename T>
    math::Point3<T> Bounds3<T>::corner(std::size_t corner) const {
        assert(corner < 8);
        return math::Point3<T>{(*this)[(corner & 1)].x,
                               (*this)[(corner & 2) ? 1 : 0].y,
                               (*this)[(corner & 4) ? 1 : 0].z};
    }

    template <typename T>
    math::Point3<T> lerp(const Bounds3<T>& bounds, const Point3f& t) {
        return {pbrt::lerp(t.x, bounds.min.x, bounds.max.x),
                pbrt::lerp(t.y, bounds.min.y, bounds.max.y),
                pbrt::lerp(t.z, bounds.min.z, bounds.max.z)};
    }

    template <typename T>
    Bounds3<T> unionOf(const Bounds3<T>& b, const math::Point3<T>& p) {
        Bounds3<T> result;
        result.min = min(b.min, p);
        result.max = max(b.max, p);
        return result;
    }

    template <typename T>
    Bounds3<T> unionOf(const Bounds3<T>& a, const Bounds3<T>& b) {
        Bounds3<T> result;
        result.min = min(a.min, b.min);
        result.max = max(a.max, b.max);
        return result;
    }

    template <typename T>
    Bounds3<T> intersectionOf(const Bounds3<T>& b1, const Bounds3<T>& b2) {
        // Important: assign to min/max directly and don't call the Bounds3()
        // constructor, since it takes min/max of the points passed to it.  In
        // turn, that breaks returning an invalid bound for the case where we
        // intersect non-overlapping bounds (as we'd like to happen).
        Bounds3<T> result;
        result.min = max(b1.min, b2.min);
        result.max = min(b1.max, b2.max);
        return result;
    }

    template <typename T>
    bool overlap(const Bounds3<T>& left, const Bounds3<T>& right) noexcept {
        const auto overlapX =
            (left.max.x >= right.min.x) && (left.min.x <= right.max.x);
        const auto overlapY =
            (left.max.y >= right.min.y) && (left.min.y <= right.max.y);
        const auto overlapZ =
            (left.max.z >= right.min.z) && (left.min.z <= right.max.z);
        return overlapX && overlapY && overlapZ;
    }

    template <typename T>
    bool inside(const math::Point3<T>& p, const Bounds3<T>& bounds) noexcept {
        return p.x >= bounds.min.x && p.x <= bounds.max.x &&
               p.y >= bounds.min.y && p.y <= bounds.max.y &&
               p.z >= bounds.min.z && p.z <= bounds.max.z;
    }

    template <typename T>
    bool insideExclusive(const math::Point3<T>& p,
                         const Bounds3<T>& bounds) noexcept {
        return p.x >= bounds.min.x && p.x < bounds.max.x &&
               p.y >= bounds.min.y && p.y < bounds.max.y &&
               p.z >= bounds.min.z && p.z < bounds.max.z;
    }

    template <typename T, typename U>
    Bounds3<T> expand(const Bounds3<T>& bounds, U delta) {
        static_assert(std::is_arithmetic_v<U>,
                      "Cannot expand with non-arithmetic type");
        const auto v = math::Vector3<T>{delta, delta, delta};
        return Bounds3<T>{bounds.min - v, bounds.max + v};
    }

    template <typename T, typename U>
    inline Float distance(const math::Point3<T>& p, const Bounds3<U>& b) {
        return std::sqrt(distanceSquared(p, b));
    }

    template <typename T, typename U>
    Float distanceSquared(const math::Point3<T>& p, const Bounds3<U>& b) {
        const auto dx = std::max({Float{0}, b.min.x - p.x, p.x - b.max.x});
        const auto dy = std::max({Float{0}, b.min.y - p.y, p.y - b.max.y});
        const auto dz = std::max({Float{0}, b.min.z - p.z, p.z - b.max.z});
        return dx * dx + dy * dy + dz * dz;
    }

    template <typename T>
    inline bool operator==(const Bounds3<T>& a, const Bounds3<T>& b) {
        return a.min == b.min && a.max == b.max;
    }

    template <typename T>
    inline bool operator!=(const Bounds3<T>& a, const Bounds3<T>& b) {
        return !(a == b);
    }
} // namespace idragnev::pbrt