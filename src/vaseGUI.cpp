#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <algorithm>
#include <wex.h>
#include "flower.h"
#include "cVase.h"

class cGUI
{
public:
    cGUI()
        : fm(wex::maker::make())
    {
        fm.move({50, 50, 1000, 500});
        fm.text("Vase");

        registerEventHandlers();

        fm.show();
        fm.run();
    }

private:
    wex::gui &fm;

    raven::sim::gui::cFlowerFactory myFactory;

    raven::sim::gui::cVase myVase;

    void registerEventHandlers()
    {
        fm.events().draw(
            [&](PAINTSTRUCT &ps)
            {
                wex::shapes S(ps);
                draw(S);
            });

        fm.events().clickRight(
            [&]()
            {
                ConstructFlower();
            });
    }
    void draw(wex::shapes &S);

    void ConstructFlower();
};

void cGUI::draw(wex::shapes &S)
{
    for( raven::sim::gui::cFlower* flower : myVase )
    {
        S.rectangle({flower->getLocationX(),flower->getLocationY(),30,30});
    }
}

void cGUI::ConstructFlower()
{
    static wex::sMouse ms;
    ms = fm.getMouseStatus();
    std::cout <<"mouse at " << ms.x <<" "<< ms.y << "\n";
    wex::menu m(fm);
    for (auto flower : myFactory.dictionary())
    {
        m.append(flower.myName,
                 [&](const std::string &title)
                 {
                     if (!myVase.Add(title))
                         return;
                    std::cout <<"flower at "<< ms.x <<" "<< ms.y << "\n";
                     myVase.getSelected()->setLocationTopLeft(ms.x,ms.y);
                     fm.update();
                 });
    }

    std::cout <<"popup at "<< ms.x <<" "<< ms.y << "\n";
    m.popup(ms.x, ms.y);
}

main()
{
    cGUI theGUI;
    return 0;
}
