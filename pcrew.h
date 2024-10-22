#pragma once

#include <vector>
#include "phuman.h"
#include "passert.h"

namespace PCrew
{

///////////////////////////////////////////////////////////////////////////////

class Crew
{
private:
    std::vector<PHuman::Human> _humans;

public:
    Crew();
    inline std::vector<PHuman::Human> &GetCrewMembers();
    inline const std::vector<PHuman::Human> &GetCrewMembers() const;
    bool IsAnyCrewOnShuttle() const;
    PHuman::Human * ModifyRandomAliveCrewOnStarship();
    bool IsHumanWithRoleOnLocation(const PHuman::HumanRole &role, const PHuman::HumanLocation &location) const;
    const PHuman::Human * GetHumanWithRole(const PHuman::HumanRole &role) const;
    PHuman::Human * ModifyHumanWithRole(const PHuman::HumanRole &role);
    void KillAll();
};

///////////////////////////////////////////////////////////////////////////////

Crew::Crew():
    _humans(std::vector<PHuman::Human>
{
    PHuman::Human(PHuman::HumanRole::COMMANDER),
           PHuman::Human(PHuman::HumanRole::NAVIGATOR),
           PHuman::Human(PHuman::HumanRole::SCIENTIST),
           PHuman::Human(PHuman::HumanRole::SOLDIER),
           PHuman::Human(PHuman::HumanRole::SCOUT),
           PHuman::Human(PHuman::HumanRole::ENGINEER),
           PHuman::Human(PHuman::HumanRole::DOCTOR)
})
{
}

///////////////////////////////////////////////////////////////////////////////

std::vector<PHuman::Human> &Crew::GetCrewMembers()
{
    return _humans;
}

///////////////////////////////////////////////////////////////////////////////

const std::vector<PHuman::Human> &Crew::GetCrewMembers() const
{
    return _humans;
}

///////////////////////////////////////////////////////////////////////////////

bool Crew::IsAnyCrewOnShuttle() const
{
    return std::find_if(_humans.begin(), _humans.end(),
                        [](const PHuman::Human & h)
    {
        return h.GetLocation() == PHuman::HumanLocation::SHUTTLE;
    }
                       ) != _humans.end();
}

///////////////////////////////////////////////////////////////////////////////

PHuman::Human * Crew::ModifyRandomAliveCrewOnStarship()
{

    size_t n_live_crew = 0;
    size_t i;
    for(i = 0; i < _humans.size(); i++)
    {
        if(
            (_humans[i].GetLocation() == PHuman::HumanLocation::STARSHIP)
            &&
            (_humans[i].GetRole() != PHuman::HumanRole::DEAD)
        )
        {
            n_live_crew++;
        }
    }
    if(n_live_crew == 0)
    {
        return NULL;
    }

    size_t crew_numerator = 0;
    const size_t selected_crew = PRandom::IRandom(0, n_live_crew - 1);
    for(i = 0; i < _humans.size(); i++)
    {
        if(
            (_humans[i].GetLocation() == PHuman::HumanLocation::STARSHIP)
            &&
            (_humans[i].GetRole() != PHuman::HumanRole::DEAD)
        )
        {
            if(crew_numerator == selected_crew)
            {
                return &_humans[i];
            }
            crew_numerator++;
        }
    }

    PASSERT(0);
    return NULL;
}

///////////////////////////////////////////////////////////////////////////////

bool Crew::IsHumanWithRoleOnLocation(const PHuman::HumanRole &role, const PHuman::HumanLocation &location) const
{
    const std::vector<PHuman::Human>::const_iterator it = std::find_if(_humans.begin(), _humans.end(),
            [&](const PHuman::Human & h)
    {
        return (h.GetRole() == role) && (h.GetLocation() == location);
    }
                                                                      );

    return it != _humans.end();
}

///////////////////////////////////////////////////////////////////////////////

const PHuman::Human * Crew::GetHumanWithRole(const PHuman::HumanRole &role) const
{
    const std::vector<PHuman::Human>::const_iterator it = std::find_if(_humans.begin(), _humans.end(),
            [&](const PHuman::Human & h)
    {
        return h.GetRole() == role;
    }
                                                                      );

    if(it != _humans.end())
    {
        return &*it;
    }
    return NULL;
}

///////////////////////////////////////////////////////////////////////////////

PHuman::Human * Crew::ModifyHumanWithRole(const PHuman::HumanRole &role)
{
    const std::vector<PHuman::Human>::iterator it =
        std::find_if(_humans.begin(), _humans.end(),
                     [&](const PHuman::Human & h)
    {
        return h.GetRole() == role;
    }
                    );

    if(it != _humans.end())
    {
        return &*it;
    }
    return NULL;
}

///////////////////////////////////////////////////////////////////////////////

void Crew::KillAll()
{
    std::for_each(_humans.begin(), _humans.end(),
                  [&](PHuman::Human & h)
    {
        h.ApplyDeath();
    }
                 );
}

///////////////////////////////////////////////////////////////////////////////

}
