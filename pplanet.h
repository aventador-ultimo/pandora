#pragma once

#include <string>
#include <vector>
#include <random>
#include "passert.h"
#include "prandom.h"

namespace PPlanet
{

///////////////////////////////////////////////////////////////////////////////

enum class PlanetDistanceToSun : uint8_t
{
    UNKNOWN = 0,
    CLOSE,
    BIOSPHERE,
    FAR,
    REMOTE
};

enum class PlanetSize : uint8_t
{
    UNKNOWN = 0,
    TINY,
    SMALL,
    LARGE,
    GIANT
};

enum class PlanetGravity : uint8_t
{
    UNKNOWN = 0,
    MINIMAL,
    LIGHT,
    NORMAL,
    HEAVY,
    OPPRESSIVE
};

enum class PlanetAtmosphere : uint8_t
{
    UNKNOWN = 0,
    NONE,
    THIN,
    NORMAL,
    TOXIC,
    CORROSIVE
};

enum class PlanetHydrography : uint8_t
{
    UNKNOWN = 0,
    H000,
    H025,
    H050,
    H075,
    H100
};

enum class PlanetClimate : uint8_t
{
    UNKNOWN = 0,
    ARCTIC,
    TEMPERATE,
    TROPICAL,
    DESERT
};

enum class PlanetFog : uint8_t
{
    UNKNOWN = 0,
    CLEAR,
    FOG
};

enum class PlanetSeismology : uint8_t
{
    UNKNOWN = 0,
    CALM,
    ACTIVE
};

///////////////////////////////////////////////////////////////////////////////

struct RandomPlanetTraits
{
    PlanetGravity gravity;
    PlanetAtmosphere atmosphere;
    PlanetHydrography hydrography;
    PlanetSeismology seismology;
    int resource_consumption;
    inline RandomPlanetTraits()
    {
    }
    inline RandomPlanetTraits(
        const PlanetGravity &gravity_p,
        const PlanetAtmosphere &atmosphere_p,
        const PlanetHydrography &hydrography_p,
        const PlanetSeismology &seismology_p,
        int resource_consumption_p
    )
        :
        gravity(gravity_p),
        atmosphere(atmosphere_p),
        hydrography(hydrography_p),
        seismology(seismology_p),
        resource_consumption(resource_consumption_p)
    {
        PASSERT(resource_consumption >= 0);
        PASSERT(resource_consumption <= 4);
    }
};

///////////////////////////////////////////////////////////////////////////////

//TODO: setting status to visited
class Planet
{
private:
    std::string _planet_name;
    PlanetDistanceToSun _planet_distance2sun;
    PlanetSize _planet_size;
    int _distance_moves_startfield;
    std::vector<RandomPlanetTraits> _random_traits;

    PlanetGravity _planet_gravity;
    PlanetAtmosphere _planet_atmosphere;
    PlanetHydrography _planet_hydrography;
    PlanetSeismology _planet_seismology;
    int _resource_consumption;

    PlanetClimate _planet_climate;
    PlanetFog _planet_fog;

    bool _discovered;
    bool _visited;// For Scientist madness
    int _supply_check_period;

public:
    inline Planet();
    Planet(
        const std::string &planet_name,
        const PlanetDistanceToSun &planet_distance2sun,
        const PlanetSize &planet_size,
        int distance_moves_startfield,
        const std::vector<RandomPlanetTraits> &random_traits
    );
    void SetGravity(const PlanetGravity &planet_gravity);
    void SetAtmosphere(const PlanetAtmosphere &planet_atmosphere);
    void Discover();
    inline const PlanetGravity &GetPlanetGravity() const;
    inline const PlanetAtmosphere &GetPlanetAtmosphere() const;
    inline const PlanetSeismology &GetPlanetSeismology() const;
    bool IsVisited() const;
    inline const std::string &GetPlanetName() const;
    inline bool IsDiscovered() const;

private:
    void _invariant() const;
};

///////////////////////////////////////////////////////////////////////////////

void Planet::_invariant() const
{
    PASSERT(_distance_moves_startfield > 0);
    if(_discovered == true)
    {
        PASSERT(_planet_gravity != PlanetGravity::UNKNOWN);
        PASSERT(_planet_atmosphere != PlanetAtmosphere::UNKNOWN);
        PASSERT(_supply_check_period > 0);
    }
    if(_visited == true)
    {
        PASSERT(_planet_hydrography != PlanetHydrography::UNKNOWN);
        PASSERT(_planet_seismology != PlanetSeismology::UNKNOWN);
        PASSERT(_planet_climate != PlanetClimate::UNKNOWN);
        PASSERT(_planet_fog != PlanetFog::UNKNOWN);
        PASSERT(_resource_consumption >= 0);
        PASSERT(_resource_consumption <= 5);// 4 + 1 possible on specific landing paragraph
    }
    if(_planet_climate == PlanetClimate::DESERT)
    {
        PASSERT(_planet_fog == PlanetFog::CLEAR);
        PASSERT(_planet_hydrography == PlanetHydrography::H000);
    }
    if(_planet_hydrography == PlanetHydrography::H000)
    {
        PASSERT(_planet_fog == PlanetFog::CLEAR);
    }
}

///////////////////////////////////////////////////////////////////////////////

inline Planet::Planet()
{
}

Planet::Planet(
    const std::string &planet_name,
    const PlanetDistanceToSun &planet_distance2sun,
    const PlanetSize &planet_size,
    int distance_moves_startfield,
    const std::vector<RandomPlanetTraits> &random_traits
):
    _planet_name(planet_name),
    _planet_distance2sun(planet_distance2sun),
    _planet_size(planet_size),
    _distance_moves_startfield(distance_moves_startfield),
    _random_traits(random_traits),
    _planet_gravity(PlanetGravity::UNKNOWN),
    _planet_atmosphere(PlanetAtmosphere::UNKNOWN),
    _planet_hydrography(PlanetHydrography::UNKNOWN),
    _planet_seismology(PlanetSeismology::UNKNOWN),
    _resource_consumption(0),
    _planet_climate(PlanetClimate::UNKNOWN),
    _planet_fog(PlanetFog::UNKNOWN),
    _discovered(false),
    _visited(false),
    _supply_check_period(0)
{
    _invariant();
}

void Planet::SetGravity(const PlanetGravity &planet_gravity)
{
    _planet_gravity = planet_gravity;
}

void Planet::SetAtmosphere(const PlanetAtmosphere &planet_atmosphere)
{
    _planet_atmosphere = planet_atmosphere;
}

void Planet::Discover()
{
    _invariant();
    if(_discovered == true)
    {
        return;
    }

    PASSERT(_random_traits.size() == 3);// According to original game. Algorithm below supports absence or any combination.
    if(_random_traits.size() > 0)
    {
        std::uniform_int_distribution<int> traits_distribution(0, _random_traits.size() - 1);
        const RandomPlanetTraits &planet_traits = _random_traits[traits_distribution(PRandom::GENERATOR)];
        _planet_gravity = planet_traits.gravity;
        _planet_atmosphere = planet_traits.atmosphere;
        _planet_hydrography = planet_traits.hydrography;
        _planet_seismology = planet_traits.seismology;
        _resource_consumption = planet_traits.resource_consumption;
    }
    else
    {
        // Default geology:
        switch(_planet_size)
        {
        case PlanetSize::TINY:
            _planet_gravity = PlanetGravity::MINIMAL;
            _planet_atmosphere = PlanetAtmosphere::NONE;
            break;

        case PlanetSize::SMALL:
            _planet_gravity = PlanetGravity::LIGHT;
            _planet_atmosphere = PlanetAtmosphere::THIN;
            break;

        case PlanetSize::LARGE:
            _planet_gravity = PlanetGravity::NORMAL;
            _planet_atmosphere = PlanetAtmosphere::CORROSIVE;
            break;

        case PlanetSize::GIANT:
            _planet_gravity = PlanetGravity::OPPRESSIVE;
            _planet_atmosphere = PlanetAtmosphere::TOXIC;
            break;

        default:
            PASSERT(0);
            break;
        };
        _planet_hydrography = PlanetHydrography::H025;
        _planet_seismology = PlanetSeismology::CALM;
        _resource_consumption = 2;
    }

    //TODO: Define remaining traits upon landing
    //Note: no fog under water
    _planet_climate = PlanetClimate::DESERT;
    _planet_fog = PlanetFog::CLEAR;

    switch(_planet_gravity)
    {
    case PlanetGravity::MINIMAL:
        _supply_check_period = 30;
        break;

    case PlanetGravity::LIGHT:
        _supply_check_period = 22;
        break;

    case PlanetGravity::NORMAL:
        _supply_check_period = 16;
        break;

    case PlanetGravity::HEAVY:
        _supply_check_period = 12;
        break;

    case PlanetGravity::OPPRESSIVE:
        _supply_check_period = 6;
        break;

    default:
        PASSERT(0);
        break;
    };

    _discovered = true;

    _invariant();
}

inline const PlanetGravity &Planet::GetPlanetGravity() const
{
    _invariant();
    return _planet_gravity;
}

inline const PlanetAtmosphere &Planet::GetPlanetAtmosphere() const
{
    _invariant();
    return _planet_atmosphere;
}

inline const PlanetSeismology &Planet::GetPlanetSeismology() const
{
    _invariant();
    return _planet_seismology;
}

bool Planet::IsVisited() const
{
    return _visited;
}

inline const std::string &Planet::GetPlanetName() const
{
    return _planet_name;
}

inline bool Planet::IsDiscovered() const
{
    return _discovered;
}

std::ostream &operator<<(std::ostream &os, const Planet &planet)
{
    os << "Planet " << planet.GetPlanetName();

    os << " atm";
    switch(planet.GetPlanetAtmosphere())
    {
    case PlanetAtmosphere::UNKNOWN:
        os << "Unknown  ";
        break;

    case PlanetAtmosphere::NONE:
        os << "None     ";
        break;

    case PlanetAtmosphere::THIN:
        os << "Thin     ";
        break;

    case PlanetAtmosphere::NORMAL:
        os << "Normal   ";
        break;

    case PlanetAtmosphere::TOXIC:
        os << "Toxic    ";
        break;

    case PlanetAtmosphere::CORROSIVE:
        os << "Corrosive";
        break;

    default:
        PASSERT(0);
        break;
    };

    os << " grv";
    switch(planet.GetPlanetGravity())
    {
    case PlanetGravity::UNKNOWN:
        os << "Unknown   ";
        break;

    case PlanetGravity::MINIMAL:
        os << "Minimal   ";
        break;

    case PlanetGravity::LIGHT:
        os << "Light     ";
        break;

    case PlanetGravity::NORMAL:
        os << "Normal    ";
        break;

    case PlanetGravity::HEAVY:
        os << "Heavy     ";
        break;

    case PlanetGravity::OPPRESSIVE:
        os << "Oppressive";
        break;

    default:
        PASSERT(0);
        break;
    };

    return os;
}

///////////////////////////////////////////////////////////////////////////////

}
