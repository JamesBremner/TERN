#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <algorithm>
#include <wex.h>
#include <inputbox.h>
#include "flower.h"
#include "cVase.h"

class cGUI
{
public:
    cGUI()
        : fm(wex::maker::make()),
          myfDragDisable(false)
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

    bool myfDragDisable;

    void registerEventHandlers();
    void onRightClick();
    void rename();

    void draw(wex::shapes &S);

    void ConstructFlower();
    void SelectFlower();
};

void cGUI::registerEventHandlers()
{
    fm.events().draw(
        [&](PAINTSTRUCT &ps)
        {
            wex::shapes S(ps);
            draw(S);
        });

    fm.events().click(
        [&]()
        {
            SelectFlower();
        });

    fm.events().clickRight(
        [&]()
        {
            onRightClick();
            fm.update();
        });

    fm.events().mouseMove(
        [&](wex::sMouse &m)
        {
            if (!m.left)
                return;
            if (!myVase.IsSelected())
                return;
            if (myfDragDisable)
            {
                myfDragDisable = false;
                return;
            }
            myVase.getSelected()->setLocationCenter(m.x, m.y);
            fm.update();
        });
}

void cGUI::onRightClick()
{
    auto ms = fm.getMouseStatus();
    auto *clickedflower = myVase.find(ms.x, ms.y);
    if (clickedflower == nullptr)
    {
        // clicked on empty canvas, construct new flower there
        ConstructFlower();
        return;
    }
    auto *selectedflower = myVase.getSelected();
    if (selectedflower)
        if (clickedflower != selectedflower)
        {
            // clicked on flower other than selected
            // connected selected flower to clicked flower
            selectedflower->Connect(clickedflower);
            return;
        }

    // clicked on selected flower

    wex::menu m(fm);
    m.append("Rename",
             [&](const std::string &title)
             {
                 rename();
                 fm.update();
             });
    m.append("Delete",
             [&](const std::string &title)
             {
                 myVase.Delete();
                 fm.update();
             });
    m.popup(ms.x, ms.y);
    return;
}

void cGUI::rename()
{
    // prompt user to change the name of the selected flower
    auto *selectedflower = myVase.getSelected();
    wex::inputbox ib(fm);
    ib.add("Name", selectedflower->getName());
    ib.showModal();

    // rename the flower with value entered into inputbox
    selectedflower->setName(ib.value("Name"));

    /*  When the user (left) clicks on the OK button
    to dismiss the inputbox
    a mouse move event with left button down
    is queued up and handled AFTER this routine completes.
    So, it is necessary to disable the next mouse move event
    */
    myfDragDisable = true;
}

void cGUI::draw(wex::shapes &S)
{
    S.textHeight(12);
    for (raven::sim::gui::cFlower *flower : myVase)
    {
        if (flower->isSelected())
            S.color(0x0000FF);
        else
            S.color(0);
        S.rectangle({flower->getLocationX(), flower->getLocationY(), 50, 50});
        S.text(flower->getName(),
               {flower->getLocationX() + 10, flower->getLocationY() + 25});

        auto *dstFlower = flower->getDestination();
        if (dstFlower)
        {
            S.color(0xFF0000);
            POINT exitPort, entryPort;
            exitPort.x = flower->getLocationX() + 50;
            exitPort.y = flower->getLocationY() + 25;
            entryPort.x = dstFlower->getLocationX();
            entryPort.y = dstFlower->getLocationY() + 25;
            S.line({exitPort.x, exitPort.y, entryPort.x, entryPort.y});
        }
    }
}

void cGUI::ConstructFlower()
{
    static wex::sMouse ms;
    ms = fm.getMouseStatus();
    // std::cout << "mouse at " << ms.x << " " << ms.y << "\n";
    wex::menu m(fm);
    for (auto flower : myFactory.dictionary())
    {
        m.append(flower.myName,
                 [&](const std::string &title)
                 {
                     if (!myVase.Add(title))
                         return;
                     // std::cout << "flower at " << ms.x << " " << ms.y << "\n";
                     myVase.getSelected()->setLocationTopLeft(ms.x, ms.y);
                     fm.update();
                 });
    }

    // std::cout << "popup at " << ms.x << " " << ms.y << "\n";
    m.popup(ms.x, ms.y);
}

void cGUI::SelectFlower()
{
    auto ms = fm.getMouseStatus();
    auto *flower = myVase.find(ms.x, ms.y);
    if (flower == nullptr)
        return;
    myVase.setSelected(flower);
    fm.update();
}

main()
{
    cGUI theGUI;
    return 0;
}
