#pragma once

#include <ostream>
#include <algorithm>
#include "psupply.h"
#include "pplanet.h"
#include "prandom.h"
#include "passert.h"

namespace PHuman
{

///////////////////////////////////////////////////////////////////////////////

enum class HumanRole : uint8_t
{
    UNKNOWN = 0,
    COMMANDER,
    NAVIGATOR,
    SCIENTIST,
    SOLDIER,
    SCOUT,
    ENGINEER,
    DOCTOR,
    BRAIN_DAMAGED,//-4 PTS
    DEAD//-10 PTS
};

///////////////////////////////////////////////////////////////////////////////

enum class HumanLocation : uint8_t
{
    UNKNOWN,
    STARSHIP,
    SHUTTLE,
    ROVER,
    FOOT
};

///////////////////////////////////////////////////////////////////////////////

enum class HumanProtectionSuit : uint8_t
{
    UNKNOWN,
    NONE,
    ENVIROSUIT,
    ENVIROSUIT_DAMAGED,
    HEAVYSUIT,
    HEAVYSUIT_DAMAGED
};

///////////////////////////////////////////////////////////////////////////////

class Human: public PSupply::SupplyConsumer
{
private:
    HumanRole _role;
    HumanLocation _human_location;
    HumanProtectionSuit _protection_suit;
    const PPlanet::Planet * _orbiting_planet;

    int _endurance; // 0-6, 0 is dead, -1PTS per lost point
    int _intelligence; // 7-9
    int _capture;
    int _attack;
    int _mass;
    int _port;
    int _speed;

public:
    Human();
    explicit Human(const HumanRole &human_role);
    void SetOrbitingPlanet(const PPlanet::Planet * orbiting_planet);
    inline void ConfigureNoSuit();
    inline void ConfigureEnviroSuit();
    inline void ConfigureHeavySuit();
    void ApplySupportCheck();
    void ApplyCraziness();
    void ApplyDeath();
    void ApplyHibernationFailure();
    void ApplyRemoveEndurancePoint();

    inline HumanRole GetRole() const;
    inline HumanLocation GetLocation() const;
    inline int GetEndurance() const;
    inline int GetLostEndurance() const;
    inline int GetIntelligence() const;
    inline int GetCapture() const;
    inline int GetAttack() const;
    int GetMass() const;
    int GetPort() const;
    int GetSpeed() const;
    inline HumanProtectionSuit GetProtectionSuit() const;

    inline virtual int GetSupplyConsumption() const;

private:
    void _invariant() const;
};

///////////////////////////////////////////////////////////////////////////////

void Human::_invariant() const
{
    if(_role == HumanRole::DEAD)
    {
        PASSERT(_endurance == 0);
    }
    else
    {
        PASSERT(_endurance >= 0);
        PASSERT(_endurance <= 6);
        PASSERT(_intelligence >= 0);
        PASSERT(_intelligence <= 9);
        PASSERT(_capture >= 0);
        PASSERT(_attack >= 0);
        PASSERT(_port >= 0);
        PASSERT(_speed >= 0);
    }
}

///////////////////////////////////////////////////////////////////////////////

Human::Human():
    SupplyConsumer(2),
    _role(HumanRole::UNKNOWN),
    _human_location(HumanLocation::UNKNOWN),
    _orbiting_planet(NULL),
    _endurance(6),
    _intelligence(PRandom::GenerateHumanIntelligence())
{
}

Human::Human(const HumanRole &human_role):
    SupplyConsumer(2),
    _role(human_role),
    _human_location(HumanLocation::STARSHIP),
    _protection_suit(HumanProtectionSuit::NONE),
    _orbiting_planet(NULL),
    _endurance(6),
    _intelligence(PRandom::GenerateHumanIntelligence())
{
    switch(_role)
    {
    case HumanRole::COMMANDER:
        _capture = 3;
        _attack = 3;
        _mass = 5;
        _port = 5;
        _speed = 8;
        break;

    case HumanRole::NAVIGATOR:
        _capture = 3;
        _attack = 2;
        _mass = 6;
        _port = 6;
        _speed = 7;
        break;

    case HumanRole::SCIENTIST:
        _capture = 4;
        _attack = 2;
        _mass = 6;
        _port = 5;
        _speed = 6;
        break;

    case HumanRole::SOLDIER:
        _capture = 4;
        _attack = 3;
        _mass = 7;
        _port = 7;
        _speed = 8;
        break;

    case HumanRole::SCOUT:
        _capture = 3;
        _attack = 2;
        _mass = 7;
        _port = 6;
        _speed = 6;
        break;

    case HumanRole::ENGINEER:
        _capture = 3;
        _attack = 2;
        _mass = 6;
        _port = 6;
        _speed = 9;
        break;

    case HumanRole::DOCTOR:
        _capture = 3;
        _attack = 2;
        _mass = 6;
        _port = 5;
        _speed = 6;
        break;

    default:
        PASSERT(0);
        break;
    };
    _invariant();
}

///////////////////////////////////////////////////////////////////////////////

void Human::SetOrbitingPlanet(const PPlanet::Planet * orbiting_planet)
{
    _invariant();
    _orbiting_planet = orbiting_planet;//NULL allowed, means open space
    _invariant();
}

void Human::ApplySupportCheck()
{
    PASSERT(
        _human_location == HumanLocation::SHUTTLE
        ||
        _human_location == HumanLocation::ROVER
        ||
        _human_location == HumanLocation::FOOT
    );

    PASSERT(_orbiting_planet != NULL);

    if(_human_location != HumanLocation::SHUTTLE)
    {
        if(
            _orbiting_planet->GetPlanetAtmosphere() == PPlanet::PlanetAtmosphere::CORROSIVE
            &&
            _protection_suit != HumanProtectionSuit::HEAVYSUIT
        )
        {
            if(_protection_suit == HumanProtectionSuit::ENVIROSUIT) // Evirosuit too fragile to protect
            {
                _protection_suit = HumanProtectionSuit::ENVIROSUIT_DAMAGED;
            }
            ApplyDeath();
        }
        if(
            (
                _orbiting_planet->GetPlanetAtmosphere() == PPlanet::PlanetAtmosphere::NONE
                ||
                _orbiting_planet->GetPlanetAtmosphere() == PPlanet::PlanetAtmosphere::TOXIC
            )
            &&
            _protection_suit != HumanProtectionSuit::ENVIROSUIT
            &&
            _protection_suit != HumanProtectionSuit::HEAVYSUIT
        )
        {
            ApplyRemoveEndurancePoint();
            ApplyRemoveEndurancePoint();
        }
    }
}

void Human::ApplyCraziness()
{
    PASSERT(_role != HumanRole::DEAD);
    if(_role == HumanRole::DEAD)
    {
        return;
    }

    _invariant();
    _endurance = 0;// Yet still alive
    _intelligence = 0;
    _capture = 0;
    _attack = 0;
    _port = 0;
    _speed = 0;
    _role = HumanRole::BRAIN_DAMAGED;
    _invariant();
}

void Human::ApplyDeath()
{
    PASSERT(_role != HumanRole::DEAD);
    if(_role == HumanRole::DEAD)
    {
        return;
    }

    _invariant();
    _endurance = 0;
    _intelligence = 0;
    _capture = 0;
    _attack = 0;
    _port = 0;
    _speed = 0;
    _role = HumanRole::DEAD;
    _protection_suit = HumanProtectionSuit::NONE;// Protection suits are not reused.
    _invariant();
}

void Human::ApplyHibernationFailure()
{
    _invariant();

    PASSERT(_role != HumanRole::DEAD);

    if(_role == HumanRole::DEAD)
    {
        return;
    }

    _capture--;
    if(_capture < 0)
    {
        _capture = 0;
    }

    _attack--;
    if(_attack < 0)
    {
        _attack = 0;
    }

    _port--;
    if(_port < 0)
    {
        _port = 0;
    }

    _speed--;
    if(_speed < 0)
    {
        _speed = 0;
    }

    _intelligence -= PRandom::GetOneDice();
    if(_intelligence < 0)
    {
        _intelligence = 0;
    }
    if(_intelligence <= 2)
    {
        _role = HumanRole::BRAIN_DAMAGED;
    }
    if(_speed <= 0)
    {
        _role = HumanRole::BRAIN_DAMAGED;
    }

    _invariant();
}

void Human::ApplyRemoveEndurancePoint()
{
    _invariant();

    PASSERT(_role != HumanRole::DEAD);
    if(_endurance > 0)
    {
        _endurance--;
        if(_endurance == 0)
        {
            ApplyDeath();
        }
    }
    _invariant();
}

///////////////////////////////////////////////////////////////////////////////

void Human::ConfigureNoSuit()
{
    _protection_suit = HumanProtectionSuit::NONE;
}

void Human::ConfigureEnviroSuit()
{
    _protection_suit = HumanProtectionSuit::ENVIROSUIT;
}

void Human::ConfigureHeavySuit()
{
    _protection_suit = HumanProtectionSuit::HEAVYSUIT;
}

HumanProtectionSuit Human::GetProtectionSuit() const
{
    return _protection_suit;
}

HumanRole Human::GetRole() const
{
    _invariant();
    return _role;
}

HumanLocation Human::GetLocation() const
{
    _invariant();
    return _human_location;
}

int Human::GetEndurance() const
{
    _invariant();
    return _endurance;
}

int Human::GetLostEndurance() const
{
    _invariant();
    const int lost_endurance = 6 - _endurance;
    PASSERT(lost_endurance >= 0);
    PASSERT(lost_endurance <= 6);
    return lost_endurance;
}

int Human::GetIntelligence() const
{
    _invariant();
    if(_role == HumanRole::DEAD)
    {
        return 0;
    }
    return _intelligence;
}

int Human::GetCapture() const
{
    _invariant();
    if(_role == HumanRole::DEAD)
    {
        return 0;
    }
    return _capture;
}

int Human::GetAttack() const
{
    _invariant();
    if(_role == HumanRole::DEAD)
    {
        return 0;
    }
    return _attack;
}

int Human::GetMass() const
{
    _invariant();
    if(_role == HumanRole::DEAD)
    {
        return 0;
    }
    int suit_mass = 0;
    if(
        (_protection_suit == HumanProtectionSuit::HEAVYSUIT)
        ||
        (_protection_suit == HumanProtectionSuit::HEAVYSUIT_DAMAGED)
    )
    {
        suit_mass = 9;
    }
    if(
        (_protection_suit == HumanProtectionSuit::ENVIROSUIT)
        ||
        (_protection_suit == HumanProtectionSuit::ENVIROSUIT_DAMAGED)
    )
    {
        suit_mass = 4;
    }
    return _mass + suit_mass;
}

int Human::GetPort() const
{
    _invariant();
    if(_role == HumanRole::DEAD)
    {
        return 0;
    }
    int live_port = _port;// Specific for given crew with modifiers
    PASSERT(_port <= 8);
    // Only undamaged suit can improve port:
    if(_protection_suit == HumanProtectionSuit::HEAVYSUIT)
    {
        live_port = 8;
    }
    const int lost_endurance = 6 - _endurance;
    PASSERT(lost_endurance >= 0);
    PASSERT(lost_endurance <= 6);

    // Modified rule: only without heavy suit, port can be limited:
    if(_protection_suit != HumanProtectionSuit::HEAVYSUIT)
    {
        live_port -= lost_endurance;
        if(live_port < 0)
        {
            live_port = 0;
        }
    }

    int planetary_live_port = live_port;// With planet modifiers

    if(_protection_suit == HumanProtectionSuit::NONE)
    {
        if(_orbiting_planet != NULL)
        {
            switch(_orbiting_planet->GetPlanetAtmosphere())
            {
            case PPlanet::PlanetAtmosphere::THIN:
                planetary_live_port--;
                if(planetary_live_port < 0)
                {
                    planetary_live_port = 0;
                }
                break;

            case PPlanet::PlanetAtmosphere::NONE:
            case PPlanet::PlanetAtmosphere::TOXIC:
            case PPlanet::PlanetAtmosphere::CORROSIVE:
                planetary_live_port = 0;
                break;

            default:
                break;
            }
        }
    }

    if(_orbiting_planet != NULL)
    {
        switch(_orbiting_planet->GetPlanetGravity())
        {
        case PPlanet::PlanetGravity::MINIMAL:
            planetary_live_port *= 2;
            break;

        case PPlanet::PlanetGravity::LIGHT:
            planetary_live_port += 2;
            break;

        case PPlanet::PlanetGravity::HEAVY:
            planetary_live_port -= 2;
            break;

        case PPlanet::PlanetGravity::OPPRESSIVE:
            planetary_live_port /= 2;
            break;

        default:
            break;
        };

        if(planetary_live_port < 0)
        {
            planetary_live_port = 0;
        }
    }

    if(live_port == 0)
    {
        planetary_live_port = 0;
    }
    PASSERT(planetary_live_port >= 0);
    return planetary_live_port;
}

int Human::GetSpeed() const
{
    _invariant();
    if(_role == HumanRole::DEAD)
    {
        return 0;
    }

    int suit_speed_modifier = 0;
    if(_protection_suit == HumanProtectionSuit::HEAVYSUIT)
    {
        return 7;//Replaces original speed, but only if not damaged
    }
    if(
        (_protection_suit == HumanProtectionSuit::ENVIROSUIT)
        ||
        (_protection_suit == HumanProtectionSuit::ENVIROSUIT_DAMAGED)
    )
    {
        suit_speed_modifier = -1;
    }
    int modified_speed = _speed + suit_speed_modifier;
    if(modified_speed < 0)
    {
        modified_speed = 0;
    }

    return modified_speed;
}

///////////////////////////////////////////////////////////////////////////////

int Human::GetSupplyConsumption() const
{
    _invariant();
    if(_role == HumanRole::DEAD)
    {
        return 0;
    }
    if(
        (
            (_protection_suit == HumanProtectionSuit::HEAVYSUIT)
            ||
            (_protection_suit == HumanProtectionSuit::HEAVYSUIT_DAMAGED)
        )
        &&
        (
            (_human_location == HumanLocation::ROVER)
            ||
            (_human_location == HumanLocation::FOOT)
        )
    )
    {
        return 3;// Heavy Suit consumes resouces if outside shuttle
    }
    return 2;
}

///////////////////////////////////////////////////////////////////////////////

std::ostream &operator<<(std::ostream &os, const Human &human)
{
    switch(human.GetRole())
    {
    case HumanRole::UNKNOWN:
        os << "Unknown at ";
        break;

    case HumanRole::COMMANDER:
        os << "Commander    ";
        break;
    case HumanRole::NAVIGATOR:
        os << "Navigator    ";
        break;
    case HumanRole::SCIENTIST:
        os << "Scientist    ";
        break;
    case HumanRole::SOLDIER:
        os << "Soldier      ";
        break;
    case HumanRole::SCOUT:
        os << "Scout        ";
        break;
    case HumanRole::ENGINEER:
        os << "Engineer     ";
        break;
    case HumanRole::DOCTOR:
        os << "Doctor       ";
        break;
    case HumanRole::BRAIN_DAMAGED:
        os << "Brain damaged";
        break;
    case HumanRole::DEAD:
        os << "Dead         ";
        break;

    default:
        PASSERT(0);
        break;
    };

    os << " @";
    switch(human.GetLocation())
    {
    case HumanLocation::UNKNOWN:
        os << "Unknown ";
        break;

    case HumanLocation::STARSHIP:
        os << "Starship";
        break;

    case HumanLocation::SHUTTLE:
        os << "Shuttle ";
        break;

    case HumanLocation::ROVER:
        os << "Rover   ";
        break;

    case HumanLocation::FOOT:
        os << "OnFoot  ";
        break;

    default:
        PASSERT(0);
        break;
    };

    os << " ps";

    switch(human.GetProtectionSuit())
    {
    case HumanProtectionSuit::UNKNOWN:
        os << "Unknown ";
        break;

    case HumanProtectionSuit::NONE:
        os << "n";
        break;

    case HumanProtectionSuit::ENVIROSUIT:
        os << "E";
        break;

    case HumanProtectionSuit::ENVIROSUIT_DAMAGED:
        os << "e";
        break;

    case HumanProtectionSuit::HEAVYSUIT:
        os << "H ";
        break;

    case HumanProtectionSuit::HEAVYSUIT_DAMAGED:
        os << "h";
        break;

    default:
        PASSERT(0);
        break;
    };

    os << " end" << human.GetEndurance();
    os << " int" << human.GetIntelligence();
    os << " cap" << human.GetCapture();
    os << " att" << human.GetAttack();
    os << " mas" << human.GetMass();
    os << " por" << human.GetPort();
    os << " spd" << human.GetSpeed();

    return os;
}

///////////////////////////////////////////////////////////////////////////////

}
