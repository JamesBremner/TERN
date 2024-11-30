#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <algorithm>
#include <wex.h>
#include <inputbox.h>
#include <plot2d.h>
#include "flower.h"
#include "cVase.h"

class cGUI
{
public:
    cGUI();

private:
    wex::gui &fm;
    wex::menu *myFileMenu;
    wex::menu *mySimMenu;
    wex::plot::plot &myPlot;
    wex::plot::trace &myPlotTrace;

    raven::sim::gui::cFlowerFactory myFactory;

    raven::sim::gui::cVase myVase;

    std::string mySimReport;
    std::string myDisplayReport;

    void menus();
    void registerEventHandlers();
    void onRightClick();
    void rename();

    void draw(wex::shapes &S);

    void simulate();

    void ConstructFlower();
    void SelectFlower();
    void config();
};

cGUI::cGUI()
    : fm(wex::maker::make()),
      myPlot(wex::maker::make<wex::plot::plot>(fm)),
      myPlotTrace(myPlot.AddStaticTrace())
{
    fm.move({50, 50, 1000, 500});
    fm.text("Vase");
    myPlot.move({30, 300, 1200, 600});
    myPlotTrace.color(0x0000FF);

    menus();
    registerEventHandlers();

    fm.show();
    fm.run();
}

void cGUI::menus()
{
    wex::menubar mb(fm);

    myFileMenu = new wex::menu(fm);
    myFileMenu->append(
        "Open",
        [&](const std::string &title)
        {
            wex::filebox fb(fm);
            auto fn = fb.open();
            if (fn.empty())
                return;
            myVase.Read(fn);
            fm.update();
        });
    myFileMenu->append(
        "Save",
        [&](const std::string &title)
        {
            wex::filebox fb(fm);
            auto fn = fb.save();
            if (fn.empty())
                return;
            myVase.Write(fn);
        });
    mb.append("File", *myFileMenu);

    mySimMenu = new wex::menu(fm);
    mySimMenu->append(
        "Run",
        [&](const std::string &title)
        {
            simulate();
        });
    mySimMenu->append(
        "Time",
        [&](const std::string &title)
        {
            wex::inputbox ib(fm);
            ib.text("Simulation parameters");
            ib.add("Time", std::to_string(myVase.getSimTime()));
            ib.showModal();
            myVase.setSimTime(atoi(ib.value("Time").c_str()));
        });
    mb.append("Simulate", *mySimMenu);
}

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
            fm.update();
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
            // if (!m.left)
            //     return;
            if (!m.shift)
                return;
            if (!myVase.IsSelected())
                return;
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
    m.append("Configure",
             [&](const std::string &title)
             {
                 config();
                 fm.update();
             });
    m.append("Delete",
             [&](const std::string &title)
             {
                 myVase.Delete();
                 fm.update();
             });
    m.append("Plot",
             [&](const std::string &title)
             {
                 myVase.readPlot();
                 myPlotTrace.set(myVase.getPlot());
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
}

void cGUI::draw(wex::shapes &S)
{
    if (!myDisplayReport.empty())
    {
        S.textHeight(20);
        S.text(myDisplayReport,
               {10, 10, 600, 25});
    }

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
            POINT entryPort;
            int xep, yep;
            flower->locationExitPort1( xep, yep);
            entryPort.x = dstFlower->getLocationX();
            entryPort.y = dstFlower->getLocationY() + 25;
            S.line({xep,yep, entryPort.x, entryPort.y});
        }
        dstFlower = flower->getDestination2();
        if (dstFlower)
        {
            S.color(0xFF0000);
            POINT entryPort;
            int xep, yep;
            flower->locationExitPort2( xep,yep);
            entryPort.x = dstFlower->getLocationX();
            entryPort.y = dstFlower->getLocationY() + 25;
            S.line({xep,yep, entryPort.x, entryPort.y});
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

    m.popup(ms.x, ms.y);
}

void cGUI::SelectFlower()
{
    auto ms = fm.getMouseStatus();
    auto *flower = myVase.find(ms.x, ms.y);
    if (flower == nullptr)
        return;

    myVase.setSelected(flower);

    myPlot.text(flower->getName());
    myPlotTrace.clear();

    if (mySimReport.find("Final Report") == -1 )
    {
        myDisplayReport.clear();
        return;
    }
    myDisplayReport = mySimReport.substr(mySimReport.find("Final Report"));
    myDisplayReport =
        myDisplayReport.substr(
            myDisplayReport.find(
                myVase.getSelected()->getName()));


}

void cGUI::config()
{
    wex::inputbox ib(fm);
    ib.text("Configure " +
            myVase.getSelected()->getName());

    for (auto &prm : myVase.getSelected()->myParam)
    {
        ib.add(
            prm.second.name,
            std::to_string(prm.second.value));
    }

    ib.showModal();

    for (auto &prm : myVase.getSelected()->myParam)
    {
        prm.second.value = atof(
            ib.value(prm.second.name).c_str());
    }
}

void cGUI::simulate()
{
    myVase.DBWrite();
    myVase.Write("vase.dot");
    fm.text("Simulating...");
    fm.update();
    std::string errs;
    wex::free::startProcess(
        "simEngine.exe", errs, true);
    fm.text("Vase");
    std::ifstream ifs("tern.log");
    std::stringstream buffer;
    buffer << ifs.rdbuf();
    mySimReport = buffer.str();
    int p = mySimReport.find("Final Report");
    if( p != -1 ) 
        mySimReport = mySimReport.substr(p);
    wex::msgbox(mySimReport.c_str());

    fm.update();
}

main()
{
    cGUI theGUI;
    return 0;
}
