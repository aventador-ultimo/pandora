// Not enough due to array initialization, lambda expressions:
//g++ -Wall --pedantic -Wextra -std=c++98 pandora.cpp

// Good enough:
//g++ -Wall --pedantic -Wextra -std=c++11 pandora.cpp

#include <iostream>
#include <string>
#include <vector>
#include <assert.h>
#include <algorithm>
#include <numeric>

#include "pplanet.h"
#include "prandom.h"
#include "pinventory.h"
#include "pvoyage.h"
#include "pcrew.h"
#include "phuman.h"
#include "psupply.h"
#include "passert.h"

class Robot: public PSupply::SupplyConsumer
{
private:
    int _endurance;	//0-2, 1 is damaged

public:
    Robot():
        SupplyConsumer(1),
        _endurance(2)
    {
    }

    virtual int GetSupplyConsumption() const
    {
        if(_endurance <= 1)
        {
            return 0;
        }
        return PSupply::SupplyConsumer::GetSupplyConsumption();
    }
};

class Tool: public PSupply::SupplyConsumer
{
public:
    Tool(): PSupply::SupplyConsumer(1)
    {
    }
};

///////////////////////////////////////////////////////////////////////////////

struct VictoryPointSystem
{
    float points_per_visited_planet;
    float points_per_extended_travel_month;
    float points_per_dead_crew;
    float points_per_brain_damaged_crew;
    float points_per_lost_crew_endurance;
    float points_per_damaged_equipment;
    float points_per_destroyed_equipment;
    float points_per_lifeform_rating;
    float points_per_lifeform_captured;
    float points_per_lifeform_survived;
    float points_per_special_captured;

public:
    VictoryPointSystem();
    void SetModifiedVPS();
};

VictoryPointSystem::VictoryPointSystem():
    points_per_visited_planet(1.0f),
    points_per_extended_travel_month(-5.0f),
    points_per_dead_crew(-10.0f),
    points_per_brain_damaged_crew(-4.0f),
    points_per_lost_crew_endurance(-1.0f),
    points_per_damaged_equipment(-1.0f),
    points_per_destroyed_equipment(-1.0f),
    points_per_lifeform_rating(1.0f),
    points_per_lifeform_captured(1.0f),
    points_per_lifeform_survived(0.0f),
    points_per_special_captured(1.0f)
{
}

void VictoryPointSystem::SetModifiedVPS()
{
    points_per_damaged_equipment = -0.5f;
    points_per_lifeform_rating = 0.25,
    points_per_lifeform_survived = 2.0f;
    points_per_special_captured = 2.0f;
}

///////////////////////////////////////////////////////////////////////////////

enum class PandoraGameMessage : uint8_t
{
    STAREVENT_NAVIGATION_ERROR,
    STAREVENT_ASTEROIDSTORM,
    STAREVENT_SUPERNOVA,
    STAREVENT_FORCEFIELDFAILURE,
    STAREVENT_COMPUTERFAILURE,
    STAREVENT_ALIENDIES,
    STAREVENT_CRAZYSCIENTIST,
    STAREVENT_OPOPLOTRANSMITTER,
    STAREVENT_CRAZYSCIENTIST_EASYCURE,
    STAREVENT_CRAZYSCIENTIST_CRAZY,
    STAREVENT_CRAZYSCIENTIST_DEATH,

    STAREVENT_PIRATES,
    STAREVENT_PIRATES_HAVESOLDIER,
    STAREVENT_PIRATES_HAVESOLDIER_SUCCESS,
    STAREVENT_PIRATES_BOARDING,
    STAREVENT_PIRATES_HAVECOMMANDER,
    STAREVENT_PIRATES_HAVECOMMANDER_SUCCESS,
    STAREVENT_PIRATES_HAVECOMMANDER_RANSOMORFIGHT,
    STAREVENT_PIRATES_HAVECOMMANDER_RANSOM,
    STAREVENT_PIRATES_HAVECOMMANDER_FIGHT,
    STAREVENT_PIRATES_HAVECOMMANDER_FIGHT_EASYWIN,
    STAREVENT_PIRATES_HAVECOMMANDER_FIGHT_HARDWIN,
    STAREVENT_PIRATES_HAVECOMMANDER_FIGHT_LOST,

    STAREVENT_STARVED_OUT,
    STAREVENT_HIBERNATION_FAILURE,

    GAME_START,
    GAME_OVER
};

class PandoraGame
{
private:
    int _interaction_extra_points;// From specific paragraph

    PVoyage::Voyage _voyage;
    PCrew::Crew _crew;
    std::vector<PPlanet::Planet> _planets;
    VictoryPointSystem _vps;
    std::map<PandoraGameMessage, std::string> _message;

public:
    PandoraGame();
    float GetVictoryPoints() const;
    float GetVictoryConditionPoints() const
    {
        return 2.0f * _voyage.GetPlannedMonths();
    }
    void PandoraInterstellarActions()
    {
        // Move to planet
        // Move to home, end game
        // Land on planet (equipment choice, landing zone choice)
        // Heal and research
    }
    void TravelTo()//TODO destination here
    {
        int travel_length = 1;//TODO

        bool near_opoplo = false;
        bool redirect_to_opoplo = false;
        StarEvent(travel_length, near_opoplo, &redirect_to_opoplo);

        if(redirect_to_opoplo == true)
        {
            travel_length = 2;//TODO redirect
        }

        _voyage.SpendMonths(travel_length);
        if(_voyage.GetPassedMonths() > 2 * _voyage.GetPlannedMonths())
        {
            PrintGameMessage(PandoraGameMessage::STAREVENT_STARVED_OUT);
            _crew.KillAll();
            GameOver();
        }
    }
    void StarEvent(unsigned travel_length, bool near_opoplo, bool * redirect_to_opoplo);

    void PrintGameMessage(const std::string &message) const
    {
        printf("%s\n", ("PGM: " + message).c_str());
    }
    void PrintGameMessage(const PandoraGameMessage pgs) const
    {
        try
        {
            PrintGameMessage(_message.at(pgs));
        }
        catch(...)
        {
            printf("ERROR: undefined message id=%u\n", (unsigned)pgs);
            PASSERT(0);
        }
    }
    void PrintVictoryPoints() const
    {
        printf("VictoryPoints: %.1f\n", GetVictoryPoints());
    }
    void GameOver()
    {
        //todo should block further actions
        PrintGameMessage(_message.at(PandoraGameMessage::GAME_OVER));
        PrintVictoryPoints();
    }
};

///////////////////////////////////////////////////////////////////////////////

PandoraGame::PandoraGame()
    :
    _interaction_extra_points(0)
{
    _message[PandoraGameMessage::STAREVENT_NAVIGATION_ERROR] = "During voyage longer than 2 months, due to computer error, extra month was lost";
    _message[PandoraGameMessage::STAREVENT_ASTEROIDSTORM] = "Additonal months lost navigating through asteroid storm";
    _message[PandoraGameMessage::STAREVENT_SUPERNOVA] = "Supernova explosion disrupted interstellar routes, extra month lost";
    _message[PandoraGameMessage::STAREVENT_FORCEFIELDFAILURE] = "Additonal months lost repairing failing forcefield of the starship";
    _message[PandoraGameMessage::STAREVENT_COMPUTERFAILURE] = "Additonal months lost repairing failing navigation computer";
    _message[PandoraGameMessage::STAREVENT_ALIENDIES] = "Captured alien lifeform is missing something elusive in its eco-habitat and dies";
    _message[PandoraGameMessage::STAREVENT_CRAZYSCIENTIST] = "Scientist develops mental illness coming from alien virus and is hurting people";
    _message[PandoraGameMessage::STAREVENT_CRAZYSCIENTIST_EASYCURE] = "Scientist recovers after one month";
    _message[PandoraGameMessage::STAREVENT_CRAZYSCIENTIST_CRAZY] = "Scientist becomes permanently crazy and disabled";
    _message[PandoraGameMessage::STAREVENT_CRAZYSCIENTIST_DEATH] = "Despite all efforts, scientist dies";
    _message[PandoraGameMessage::STAREVENT_OPOPLOTRANSMITTER] = "Alien transmission intercepted at Opoplo, galactic law requires investigation";

    _message[PandoraGameMessage::STAREVENT_PIRATES] = "Pirate renegades are chasing the starship";
    _message[PandoraGameMessage::STAREVENT_PIRATES_HAVESOLDIER] = "Soldier tries to face the pirates";
    _message[PandoraGameMessage::STAREVENT_PIRATES_HAVESOLDIER_SUCCESS] = "Soldier managed to outmaneuver the pirates";
    _message[PandoraGameMessage::STAREVENT_PIRATES_BOARDING] = "Space pirates are boarding";
    _message[PandoraGameMessage::STAREVENT_PIRATES_HAVECOMMANDER] = "Commader starts negotiations with pirates";
    _message[PandoraGameMessage::STAREVENT_PIRATES_HAVECOMMANDER_SUCCESS] = "Stupid pirates are leaving happily ransomed with junk, scrap metal and surplus plastic bags";
    _message[PandoraGameMessage::STAREVENT_PIRATES_HAVECOMMANDER_RANSOMORFIGHT] = "Commander can convince pirates to leave with a ransom of each equipment kit";
    _message[PandoraGameMessage::STAREVENT_PIRATES_HAVECOMMANDER_RANSOM] = "Pirates accept the ransom and leave";
    _message[PandoraGameMessage::STAREVENT_PIRATES_HAVECOMMANDER_FIGHT] = "Fights with pirates starts!";
    _message[PandoraGameMessage::STAREVENT_PIRATES_HAVECOMMANDER_FIGHT_EASYWIN] = "Easy victory against pirates, one month spent on repairs.";
    _message[PandoraGameMessage::STAREVENT_PIRATES_HAVECOMMANDER_FIGHT_HARDWIN] = "Hard earned victory against pirates...";
    _message[PandoraGameMessage::STAREVENT_PIRATES_HAVECOMMANDER_FIGHT_LOST] = "Lost against pirates...";

    _message[PandoraGameMessage::STAREVENT_STARVED_OUT] = "After many months of travel, everybody has starved out";
    _message[PandoraGameMessage::STAREVENT_HIBERNATION_FAILURE] = "One crewmemeber suffers horribly from hibernation failure";

    _message[PandoraGameMessage::GAME_START] = "Game starts";
    _message[PandoraGameMessage::GAME_OVER] = "Game over";


    using namespace PPlanet;
    const std::vector<RandomPlanetTraits> korkran_traits =
    {
        RandomPlanetTraits(PlanetGravity::MINIMAL, PlanetAtmosphere::TOXIC, PlanetHydrography::H075, PlanetSeismology::CALM, 1),
        RandomPlanetTraits(PlanetGravity::NORMAL, PlanetAtmosphere::NORMAL, PlanetHydrography::H050, PlanetSeismology::CALM, 0),
        RandomPlanetTraits(PlanetGravity::HEAVY, PlanetAtmosphere::NORMAL, PlanetHydrography::H100, PlanetSeismology::CALM, 0)
    };
    const std::vector<RandomPlanetTraits> opoplo_traits =
    {
        RandomPlanetTraits(PlanetGravity::HEAVY, PlanetAtmosphere::NORMAL, PlanetHydrography::H050, PlanetSeismology::CALM, 0),
        RandomPlanetTraits(PlanetGravity::OPPRESSIVE, PlanetAtmosphere::TOXIC, PlanetHydrography::H050, PlanetSeismology::CALM, 4),
        RandomPlanetTraits(PlanetGravity::OPPRESSIVE, PlanetAtmosphere::CORROSIVE, PlanetHydrography::H000, PlanetSeismology::CALM, 4)
    };
    const std::vector<RandomPlanetTraits> mezo_traits =
    {
        RandomPlanetTraits(PlanetGravity::NORMAL, PlanetAtmosphere::NONE, PlanetHydrography::H000, PlanetSeismology::CALM, 4),
        RandomPlanetTraits(PlanetGravity::HEAVY, PlanetAtmosphere::THIN, PlanetHydrography::H025, PlanetSeismology::CALM, 2),
        RandomPlanetTraits(PlanetGravity::HEAVY, PlanetAtmosphere::TOXIC, PlanetHydrography::H050, PlanetSeismology::ACTIVE, 3)
    };
    const std::vector<RandomPlanetTraits> paleo_traits =
    {
        RandomPlanetTraits(PlanetGravity::NORMAL, PlanetAtmosphere::THIN, PlanetHydrography::H000, PlanetSeismology::ACTIVE, 4),
        RandomPlanetTraits(PlanetGravity::HEAVY, PlanetAtmosphere::NORMAL, PlanetHydrography::H025, PlanetSeismology::ACTIVE, 1),
        RandomPlanetTraits(PlanetGravity::HEAVY, PlanetAtmosphere::CORROSIVE, PlanetHydrography::H000, PlanetSeismology::ACTIVE, 4)
    };
    const std::vector<RandomPlanetTraits> picola_traits =
    {
        RandomPlanetTraits(PlanetGravity::MINIMAL, PlanetAtmosphere::TOXIC, PlanetHydrography::H025, PlanetSeismology::CALM, 2),
        RandomPlanetTraits(PlanetGravity::MINIMAL, PlanetAtmosphere::NONE, PlanetHydrography::H025, PlanetSeismology::CALM, 3),
        RandomPlanetTraits(PlanetGravity::LIGHT, PlanetAtmosphere::THIN, PlanetHydrography::H050, PlanetSeismology::CALM, 1)
    };
    const std::vector<RandomPlanetTraits> birss_traits =
    {
        RandomPlanetTraits(PlanetGravity::LIGHT, PlanetAtmosphere::NONE, PlanetHydrography::H000, PlanetSeismology::ACTIVE, 4),
        RandomPlanetTraits(PlanetGravity::NORMAL, PlanetAtmosphere::THIN, PlanetHydrography::H000, PlanetSeismology::ACTIVE, 2),
        RandomPlanetTraits(PlanetGravity::HEAVY, PlanetAtmosphere::TOXIC, PlanetHydrography::H000, PlanetSeismology::ACTIVE, 4)
    };
    const std::vector<RandomPlanetTraits> mephisto_traits =
    {
        RandomPlanetTraits(PlanetGravity::LIGHT, PlanetAtmosphere::NORMAL, PlanetHydrography::H000, PlanetSeismology::ACTIVE, 2),
        RandomPlanetTraits(PlanetGravity::NORMAL, PlanetAtmosphere::TOXIC, PlanetHydrography::H000, PlanetSeismology::ACTIVE, 4),
        RandomPlanetTraits(PlanetGravity::HEAVY, PlanetAtmosphere::TOXIC, PlanetHydrography::H025, PlanetSeismology::ACTIVE, 3)
    };
    const std::vector<RandomPlanetTraits> newalto_traits =
    {
        RandomPlanetTraits(PlanetGravity::LIGHT, PlanetAtmosphere::THIN, PlanetHydrography::H000, PlanetSeismology::CALM, 3),
        RandomPlanetTraits(PlanetGravity::NORMAL, PlanetAtmosphere::NORMAL, PlanetHydrography::H025, PlanetSeismology::ACTIVE, 1),
        RandomPlanetTraits(PlanetGravity::HEAVY, PlanetAtmosphere::TOXIC, PlanetHydrography::H050, PlanetSeismology::ACTIVE, 3)
    };
    const std::vector<RandomPlanetTraits> suwathe_traits =
    {
        RandomPlanetTraits(PlanetGravity::NORMAL, PlanetAtmosphere::CORROSIVE, PlanetHydrography::H075, PlanetSeismology::CALM, 2),
        RandomPlanetTraits(PlanetGravity::HEAVY, PlanetAtmosphere::TOXIC, PlanetHydrography::H100, PlanetSeismology::ACTIVE, 1),
        RandomPlanetTraits(PlanetGravity::HEAVY, PlanetAtmosphere::CORROSIVE, PlanetHydrography::H050, PlanetSeismology::ACTIVE, 3)
    };

    _planets = std::vector<Planet>
    {
        Planet("Korkran", PlanetDistanceToSun::BIOSPHERE, PlanetSize::SMALL, 7, korkran_traits),
        Planet("Opoplo", PlanetDistanceToSun::FAR, PlanetSize::GIANT, 5, opoplo_traits),
        Planet("Mezo", PlanetDistanceToSun::FAR, PlanetSize::LARGE, 3, mezo_traits),
        Planet("Paleo", PlanetDistanceToSun::CLOSE, PlanetSize::LARGE, 5, paleo_traits),
        Planet("Picola", PlanetDistanceToSun::BIOSPHERE, PlanetSize::TINY, 3, picola_traits),
        Planet("Birss", PlanetDistanceToSun::REMOTE, PlanetSize::SMALL, 3, birss_traits),
        Planet("Mephisto", PlanetDistanceToSun::CLOSE, PlanetSize::SMALL, 1, mephisto_traits),
        Planet("NewAlto", PlanetDistanceToSun::FAR, PlanetSize::SMALL, 7, newalto_traits),
        Planet("Suwathe", PlanetDistanceToSun::BIOSPHERE, PlanetSize::LARGE, 3, suwathe_traits)
    };

    _vps.SetModifiedVPS();// Experimental
    PrintGameMessage("Using improved victory point system");

    PrintGameMessage(PandoraGameMessage::GAME_START);
    PrintVictoryPoints();
}

///////////////////////////////////////////////////////////////////////////////

float PandoraGame::GetVictoryPoints() const
{
    const float visited_planet_points = _vps.points_per_visited_planet * std::count_if(_planets.begin(), _planets.end(),
                                        [](const PPlanet::Planet & planet)
    {
        return planet.IsVisited();
    }
                                                                                      );
    PASSERT(visited_planet_points >= 0.0f);

    const float travel_extended_penalty = _vps.points_per_extended_travel_month *
                                          (_voyage.GetRemainingMonths() < 0 ? -_voyage.GetRemainingMonths() : 0);
    PASSERT(travel_extended_penalty <= 0.0f);

    const float dead_crew_penalty = _vps.points_per_dead_crew *
                                    std::count_if(_crew.GetCrewMembers().begin(), _crew.GetCrewMembers().end(),
                                            [](const PHuman::Human & human)
    {
        return human.GetRole() == PHuman::HumanRole::DEAD;
    }
                                                 );
    PASSERT(dead_crew_penalty <= 0.0f);

    const float brain_damaged_crew_penalty = _vps.points_per_brain_damaged_crew *
            std::count_if(_crew.GetCrewMembers().begin(), _crew.GetCrewMembers().end(),
                          [](const PHuman::Human & human)
    {
        return human.GetRole() == PHuman::HumanRole::BRAIN_DAMAGED;
    }
                         );
    PASSERT(brain_damaged_crew_penalty <= 0.0f);

    float lost_endurance_crew_penalty = 0.0f;
    std::for_each(_crew.GetCrewMembers().begin(), _crew.GetCrewMembers().end(),
                  [&](const PHuman::Human & human)
    {
        if(human.GetRole() != PHuman::HumanRole::DEAD)
        {
            lost_endurance_crew_penalty += _vps.points_per_lost_crew_endurance * human.GetLostEndurance();
        }
    }
                 );
    PASSERT(lost_endurance_crew_penalty <= 0.0f);

    //-1 for every damaged or destroyed equipment
    //-1 for every type of completely expended equipment
    //+1 for every lifeform rating
    //+1 for every lifeform survived to return
    //+N for every special lifeform bonus survived to return
    //+N for every special lifeform-product bonus
    return
        visited_planet_points
        +
        travel_extended_penalty
        +
        dead_crew_penalty
        +
        brain_damaged_crew_penalty
        +
        lost_endurance_crew_penalty
        +
        _interaction_extra_points
        ;
}

///////////////////////////////////////////////////////////////////////////////

void PandoraGame::StarEvent(unsigned travel_length, bool near_opoplo, bool * redirect_to_opoplo)
{
    PASSERT(travel_length > 0);
    PASSERT(redirect_to_opoplo != NULL);

    *redirect_to_opoplo = false;

    const unsigned star_event_case = PRandom::GenerateStarEventCase(travel_length);

    // Select paragraph: 80, 61, 55, 49, 46, 1, 44, 47, 52, 58, 64
    switch(star_event_case)
    {
    case 0:
        return;// Nothing happened

    case 1://@80
        //Alien onboard
        //TODO
        break;

    case 2://@61
        //Pirates
        PrintGameMessage(PandoraGameMessage::STAREVENT_PIRATES);
        if(_crew.IsHumanWithRoleOnLocation(PHuman::HumanRole::SOLDIER, PHuman::HumanLocation::STARSHIP) == true)
        {
            PrintGameMessage(PandoraGameMessage::STAREVENT_PIRATES_HAVESOLDIER);
            const PHuman::Human * const soldier = _crew.GetHumanWithRole(PHuman::HumanRole::SOLDIER);
            std::cout << soldier << std::endl;
            if(PRandom::GetSumTwoDices() < soldier->GetIntelligence())
            {
                PrintGameMessage(PandoraGameMessage::STAREVENT_PIRATES_HAVESOLDIER_SUCCESS);
                break;
            }
        }

        //@169
        PrintGameMessage(PandoraGameMessage::STAREVENT_PIRATES_BOARDING);
        if(_crew.IsHumanWithRoleOnLocation(PHuman::HumanRole::COMMANDER, PHuman::HumanLocation::STARSHIP) == true)
        {
            PrintGameMessage(PandoraGameMessage::STAREVENT_PIRATES_HAVECOMMANDER);
            const PHuman::Human * const commander = _crew.GetHumanWithRole(PHuman::HumanRole::COMMANDER);
            std::cout << commander << std::endl;
            const int luck = PRandom::GetSumTwoDices();
            if(luck < commander->GetIntelligence() - 1)
            {
                PrintGameMessage(PandoraGameMessage::STAREVENT_PIRATES_HAVECOMMANDER_SUCCESS);
                break;
            }
            else if(
                (luck >= commander->GetIntelligence() - 1)
                &&
                (luck <= commander->GetIntelligence() + 1)
            )
            {
                //@203
                PrintGameMessage(PandoraGameMessage::STAREVENT_PIRATES_HAVECOMMANDER_RANSOMORFIGHT);
                bool selected_RANSOM = true;//xxx todo
                if(selected_RANSOM)
                {
                    PrintGameMessage(PandoraGameMessage::STAREVENT_PIRATES_HAVECOMMANDER_RANSOM);
                    // todo: do the RANSOM
                }
            }
        }

        //@183
        PrintGameMessage(PandoraGameMessage::STAREVENT_PIRATES_HAVECOMMANDER_FIGHT);
        switch(PRandom::GetOneDice())
        {
        //@183
        case 1:
        case 2:
        case 3:
        {
            int total_hp_loss = PRandom::GetSumTwoDices();
            while(total_hp_loss > 0)
            {
                PHuman::Human * const human = _crew.ModifyRandomAliveCrewOnStarship();
                if(human != NULL)
                {
                    human->ApplyRemoveEndurancePoint();
                    total_hp_loss--;
                }
                else
                {
                    total_hp_loss = 0;
                }
            }
            PrintGameMessage(PandoraGameMessage::STAREVENT_PIRATES_HAVECOMMANDER_FIGHT_EASYWIN);
            _voyage.SpendMonths(1);
        }
        break;

        //@191
        case 4:
        case 5:
        {
            PrintGameMessage(PandoraGameMessage::STAREVENT_PIRATES_HAVECOMMANDER_FIGHT_HARDWIN);
            int total_life_loss = PRandom::GetOneDice();
            while(total_life_loss > 0)
            {
                PHuman::Human * const human = _crew.ModifyRandomAliveCrewOnStarship();
                if(human == NULL)
                {
                    GameOver();
                    break;
                }
                human->ApplyDeath();
                total_life_loss--;
            }
            //todo damages to  equipment having attack skill
            int month_loss = PRandom::GetOneDice();
            if(_crew.IsHumanWithRoleOnLocation(PHuman::HumanRole::ENGINEER, PHuman::HumanLocation::STARSHIP))
            {
                month_loss -= 2;
                if(month_loss < 1)//modified from original to keep in sync with easier scenario
                {
                    month_loss = 1;
                }
            }
            _voyage.SpendMonths(month_loss);
            if(_crew.ModifyRandomAliveCrewOnStarship() == NULL)
            {
                GameOver();
            }
        }
        break;

        //@183
        case 6:
            PrintGameMessage(PandoraGameMessage::STAREVENT_PIRATES_HAVECOMMANDER_FIGHT_LOST);
            _crew.KillAll();
            GameOver();
            break;

        default:
            PASSERT(0);
            break;
        };

        break;

    case 3://@55
    {
        PrintGameMessage(PandoraGameMessage::STAREVENT_HIBERNATION_FAILURE);
        PHuman::Human * const human = _crew.ModifyRandomAliveCrewOnStarship();
        if(human != NULL)
        {
            human->ApplyHibernationFailure();
            std::cout << *human << std::endl;
            break;
        }
    }
    break;

    case 4://@49
    {
        PrintGameMessage(PandoraGameMessage::STAREVENT_ASTEROIDSTORM);
        int best_intelligence = 0;
        PHuman::HumanRole best_role = PHuman::HumanRole::UNKNOWN;
        if(_crew.IsHumanWithRoleOnLocation(PHuman::HumanRole::COMMANDER, PHuman::HumanLocation::STARSHIP))
        {
            const int intelligence = _crew.GetHumanWithRole(PHuman::HumanRole::COMMANDER)->GetIntelligence();
            if(intelligence > best_intelligence)
            {
                best_role = PHuman::HumanRole::COMMANDER;
                best_intelligence = intelligence;
            }
        }
        if(_crew.IsHumanWithRoleOnLocation(PHuman::HumanRole::NAVIGATOR, PHuman::HumanLocation::STARSHIP))
        {
            const int intelligence = _crew.GetHumanWithRole(PHuman::HumanRole::NAVIGATOR)->GetIntelligence();
            if(intelligence > best_intelligence)
            {
                best_role = PHuman::HumanRole::NAVIGATOR;
                best_intelligence = intelligence;
            }
        }
        if(_crew.IsHumanWithRoleOnLocation(PHuman::HumanRole::ENGINEER, PHuman::HumanLocation::STARSHIP))
        {
            const int intelligence = _crew.GetHumanWithRole(PHuman::HumanRole::ENGINEER)->GetIntelligence();
            if(intelligence > best_intelligence)
            {
                best_role = PHuman::HumanRole::ENGINEER;
                best_intelligence = intelligence;
            }
        }
        std::cout << *_crew.GetHumanWithRole(best_role) << std::endl;
        PASSERT(best_intelligence <= 9);
        _voyage.SpendMonths(9 - best_intelligence);
    }
    break;

    case 5://@46
        PrintGameMessage(PandoraGameMessage::STAREVENT_SUPERNOVA);
        _voyage.SpendMonths(1);
        break;

    case 6://@1
    {
        // Computer malfunction
        if(travel_length >= 3)
        {
            PrintGameMessage(PandoraGameMessage::STAREVENT_NAVIGATION_ERROR);
            _voyage.SpendMonths(1);
        }
    }
    break;

    case 7://@44
    {
        PrintGameMessage(PandoraGameMessage::STAREVENT_FORCEFIELDFAILURE);
        int months_lost = 4;
        if(_crew.IsHumanWithRoleOnLocation(PHuman::HumanRole::ENGINEER, PHuman::HumanLocation::STARSHIP))
        {
            const int intelligence = _crew.GetHumanWithRole(PHuman::HumanRole::ENGINEER)->GetIntelligence();
            const int luck = PRandom::GetOneDice();
            if(luck <= intelligence)
            {
                months_lost = 1;
            }
            else
            {
                months_lost = luck - intelligence;
                PASSERT(months_lost > 0);
                if(months_lost > 4)
                {
                    months_lost = 4;
                }
            }
        }
        _voyage.SpendMonths(months_lost);
    }
    break;

    case 8://@47
    {
        PrintGameMessage(PandoraGameMessage::STAREVENT_COMPUTERFAILURE);
        int best_intelligence = 0;
        PHuman::HumanRole best_role = PHuman::HumanRole::UNKNOWN;
        if(_crew.IsHumanWithRoleOnLocation(PHuman::HumanRole::SCIENTIST, PHuman::HumanLocation::STARSHIP))
        {
            const int intelligence = _crew.GetHumanWithRole(PHuman::HumanRole::SCIENTIST)->GetIntelligence();
            if(intelligence > best_intelligence)
            {
                best_role = PHuman::HumanRole::SCIENTIST;
                best_intelligence = intelligence;
            }
        }
        if(_crew.IsHumanWithRoleOnLocation(PHuman::HumanRole::ENGINEER, PHuman::HumanLocation::STARSHIP))
        {
            const int intelligence = _crew.GetHumanWithRole(PHuman::HumanRole::ENGINEER)->GetIntelligence();
            if(intelligence > best_intelligence)
            {
                best_role = PHuman::HumanRole::ENGINEER;
                best_intelligence = intelligence;
            }
        }
        if(_crew.IsHumanWithRoleOnLocation(PHuman::HumanRole::SCOUT, PHuman::HumanLocation::STARSHIP))
        {
            const int intelligence = _crew.GetHumanWithRole(PHuman::HumanRole::SCOUT)->GetIntelligence();
            if(intelligence > best_intelligence)
            {
                best_role = PHuman::HumanRole::SCOUT;
                best_intelligence = intelligence;
            }
        }
        std::cout << *_crew.GetHumanWithRole(best_role) << std::endl;
        PASSERT(best_intelligence <= 9);
        _voyage.SpendMonths(9 - best_intelligence);
    }
    break;

    case 9://@52
        //todo Random captured alien dies
        PrintGameMessage(PandoraGameMessage::STAREVENT_ALIENDIES);
        break;

    case 10://@58
    {
        //Crazy scientist
        const int nvisited = std::count_if(_planets.begin(), _planets.end(),
                                           [](const PPlanet::Planet & planet)
        {
            return planet.IsVisited();
        }
                                          );
        if(nvisited > 0)
        {
            if(_crew.IsHumanWithRoleOnLocation(PHuman::HumanRole::SCIENTIST, PHuman::HumanLocation::STARSHIP))
            {
                PrintGameMessage(PandoraGameMessage::STAREVENT_CRAZYSCIENTIST);

                PHuman::Human * affected_human = _crew.ModifyHumanWithRole(PHuman::HumanRole::SCIENTIST);

                while(affected_human != NULL)
                {
                    const int intelligence = affected_human->GetIntelligence();
                    std::cout << *affected_human << std::endl;

                    int total_hp_loss = intelligence - PRandom::GetOneDice();
                    while(total_hp_loss > 0)
                    {
                        PHuman::Human * const human = _crew.ModifyRandomAliveCrewOnStarship();
                        if(human == NULL)
                        {
                            GameOver();
                            break;
                        }
                        human->ApplyRemoveEndurancePoint();
                        total_hp_loss--;
                    }
                    switch(PRandom::GetOneDice())
                    {
                    //@67
                    case 1:
                    case 2:
                    case 3:
                    {
                        PrintGameMessage(PandoraGameMessage::STAREVENT_CRAZYSCIENTIST_EASYCURE);
                        _voyage.SpendMonths(1);
                        affected_human = NULL;
                    }
                    break;

                    //@73
                    case 4:
                    case 5:
                    {
                        if(_crew.IsHumanWithRoleOnLocation(PHuman::HumanRole::DOCTOR, PHuman::HumanLocation::STARSHIP))
                        {
                            if(PRandom::GetSumTwoDices() <= _crew.GetHumanWithRole(PHuman::HumanRole::DOCTOR)->GetIntelligence())
                            {
                                PrintGameMessage(PandoraGameMessage::STAREVENT_CRAZYSCIENTIST_EASYCURE);
                                _voyage.SpendMonths(1);//departure from original
                                break;
                            }
                        }

                        PrintGameMessage(PandoraGameMessage::STAREVENT_CRAZYSCIENTIST_CRAZY);
                        affected_human->ApplyCraziness();
                        std::cout << *affected_human << std::endl;
                        _voyage.SpendMonths(1);//departure from original
                        affected_human = NULL;
                    }
                    break;

                    //@144
                    case 6:
                    {
                        PrintGameMessage(PandoraGameMessage::STAREVENT_CRAZYSCIENTIST_DEATH);
                        affected_human->ApplyDeath();
                        std::cout << *affected_human << std::endl;
                        int months_lost = PRandom::GetOneDice();
                        if(months_lost > 4)
                        {
                            months_lost = 1;//departure from original, would be 0
                        }
                        _voyage.SpendMonths(months_lost);

                        if(
                            _crew.IsHumanWithRoleOnLocation(PHuman::HumanRole::DOCTOR, PHuman::HumanLocation::STARSHIP) == true
                            &&
                            _crew.GetHumanWithRole(PHuman::HumanRole::DOCTOR)->GetIntelligence() > 6
                        )
                        {
                            affected_human = NULL;// Unfortunate roll, but disease stopped
                        }
                        else
                        {
                            affected_human = _crew.ModifyRandomAliveCrewOnStarship();// Disease spreads out
                        }
                    }
                    break;

                    default:
                        PASSERT(0);
                        break;
                    };
                }
            }
        }
    }
    break;

    case 11://@64
        //Jump to Opoplo, todo: special landing zone!
        if(near_opoplo == true)
        {
            PrintGameMessage(PandoraGameMessage::STAREVENT_OPOPLOTRANSMITTER);
            *redirect_to_opoplo = true;
        }
        break;

    default:
        PASSERT(0);
        break;
    };
    if(_crew.ModifyRandomAliveCrewOnStarship() == NULL)
    {
        GameOver();
    }
}

///////////////////////////////////////////////////////////////////////////////

using namespace PHuman;
using namespace PVoyage;
using namespace PCrew;
using namespace PSupply;
using namespace PPlanet;

///////////////////////////////////////////////////////////////////////////////

int main()
{
    //size_t i, a, g;

    using namespace PRandom;
    using namespace std;
    GenerateExplorationParagraph(false, false);
    GenerateExplorationParagraph(false, true);
    GenerateExplorationParagraph(true, false);
    GenerateExplorationParagraph(true, true);


    /*
    puts("OneDiceHistogram");
    PrintNormalizedHistogram(OneDiceHistogram(1000000));

    puts("SumTwoDicesHistogram");
    PrintNormalizedHistogram(SumTwoDicesHistogram(1000000));

    puts("AlienRatingHistogram");
    PrintNormalizedHistogram(AlienRatingHistogram(1000000));

    puts("HumanIntelligenceHistogram");
    PrintNormalizedHistogram(HumanIntelligenceHistogram(1000000));
    */

    for(int tl = 1; tl <= 15; tl++)
    {
        printf("StarEventHistogram TL=%d\n", tl);
        PrintNormalizedHistogram(StarEventHistogram(1000000, tl));
    }

    PandoraGame game;

    /*
    for(int tl = 1; tl<12; tl++)
    {
    	//TRAVEL_LENGTH = tl;
    	printf("StarEventParagraphHistogram TravelLength=%d\n", tl);
    	PrintNormalizedHistogram(StarEventParagraphHistogram(1000000));
    }
    */

    //Planet planet=planets[0];


    /*
    for(p=0; p<planets.size(); p++)
    {
    	Planet * const p_choosen_planet=&planets[p];

    */

    /*
    PASSERT(crew.IsHumanWithRoleOnLocation(PHuman::HumanRole::SOLDIER, PHuman::HumanLocation::STARSHIP)==true);
    crew.ModifyHumanWithRole(PHuman::HumanRole::SOLDIER)->ApplyDeath();
    PASSERT(crew.IsHumanWithRoleOnLocation(PHuman::HumanRole::SOLDIER, PHuman::HumanLocation::STARSHIP)==false);
    */

    /*
    for(a=0; a<5; a++)
    {
    	for_each(crew.GetCrewMembers().begin(), crew.GetCrewMembers().end(),
    		[](Human & crew_member)
    		{
    			crew_member.ApplyRemoveEndurancePoint();
    			crew_member.ApplyHibernationFailure();
    		}
    		);
    }
    */
    /*
    Planet test_planet("TestPlanet", PlanetType::BIOSPHERE, PlanetType::SMALL, 1);
    Planet * const p_choosen_planet=&test_planet;
    p_choosen_planet->Discover();
    //for(a=1; a<=5; a++)
    a=3;
    {
    	g=1;
    	//for(g=1; g<=5; g++)
    	{
    		PlanetGravity pg = (PlanetGravity)g;
    		PlanetAtmosphere pa = (PlanetAtmosphere)a;
    		p_choosen_planet->SetGravity(pg);
    		p_choosen_planet->SetAtmosphere(pa);

    		//cout<<"a="<<a<<" g="<<g<<endl;
    		cout<<*p_choosen_planet<<endl;

    		for(i=0; i<crew.GetNMembers(); i++)
    		{
    			Human & crew_member = crew.GetCrewMembers()[i];
    			crew_member.SetOrbitingPlanet(p_choosen_planet);
    			crew_member.ConfigureNoSuit();
    			if(
    				p_choosen_planet->GetPlanetAtmosphere()==PlanetAtmosphere::NONE
    				||
    				p_choosen_planet->GetPlanetAtmosphere()==PlanetAtmosphere::TOXIC
    				)
    			{
    				crew_member.ConfigureEnviroSuit();
    			}

    			if(p_choosen_planet->GetPlanetAtmosphere()==PlanetAtmosphere::CORROSIVE)
    			{
    				crew_member.ConfigureHeavySuit();
    			}

    			cout<<crew_member<<endl;
    		}
    	}
    }
    */

    return 0;
}
