/*
 * Copyright (c) 2012 by James Bremner
 * All rights reserved.
 *
 * Use license: Modified from standard BSD license.
 *
 * Redistribution and use in source and binary forms are permitted
 * provided that the above copyright notice and this paragraph are
 * duplicated in all such forms and that any documentation, advertising
 * materials, Web server pages, and other materials related to such
 * distribution and use acknowledge that the software was developed
 * by James Bremner. The name "James Bremner" may not be used to
 * endorse or promote products derived from this software without
 * specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 */

#pragma once
#ifdef WXWIDGETS
#include "wx/wx.h"
class wxPropertyGrid;
#endif
#include <string>
using std::string;
#include <map>
#include <vector>
#include <set>

namespace raven
{
    namespace sim
    {
        class cFlowerType
        {
        public:
            int myIndex;
            std::string myName;
            cFlowerType(
                int i,
                const std::string &n)
                : myIndex(i), myName(n)
            {
            }
            /** Define less than operator
            So that the set container can order the types for binary search
            */
            inline bool operator<(const cFlowerType &other) const
            {
                return myIndex < other.myIndex;
            }

            /** Define equivalence operator
            so that equivalence reuires only that the names be the same
            */
            inline bool operator==(const cFlowerType &other) const
            {
                return myName == other.myName;
            }
        };
        /** Distionary of flower types */
        class cFlowerDict
        {
        public:
            /** Construct dictionary of flower types
             */
            cFlowerDict()
            {
                // standard types

                Insert("Base");
                Insert("Pump");
                Insert("Vessel");
                Insert("Funnel");
                Insert("Source");
                Insert("SourceFlow");
                Insert("Sink");
                Insert("Delay");
                Insert("Busy");
                Insert("Queue");
                Insert("PipeBend");

                // model types

                // TODO: InsertModelTypes();
            }

            /** Find index of flower type name
            @param[in] flower_type_name
            @return index of found flower type, or -1 if not found
            */
            const int Find(const std::string &flower_type_name)
            {
                for (auto &f : myDict)
                {
                    if (f.myName == flower_type_name)
                        return f.myIndex;
                }
                return -1;
            }
            typedef std::set<cFlowerType>::iterator iterator;
            iterator begin()
            {
                return myDict.begin();
            }
            iterator end()
            {
                return myDict.end();
            }
            int Size()
            {
                return (int)myDict.size();
            }

        private:
            std::set<cFlowerType> myDict;

            /** Insert new flower type name into dictionary

            @param[in] flower_type_name
            */
            void Insert(const std::string &flower_type_name);

            /** Insert flower types specialized for the model

            This is implemented in ModelFlowers,cpp and will
            be different for each model simulated.  For the base
            model this will be a NOP

            */
            void InsertModelTypes();
        };

        class cFlowerDict;

        namespace gui
        {
            class cFlower;

            /**

            Construct flowers of various types

            */
            class cFlowerFactory
            {
            public:
                cFlowerFactory();
                void setMenu(int IDM_CreateGeneric);
                static cFlower *Construct(
                    const std::string &flower_type_name);
                static cFlower *Construct(
                    int flower_type_index);
                static int Index(
                    const std::string &flower_type_name);
                static int TypeCount();
                static cFlowerDict &dictionary();

            protected:
                static raven::sim::cFlowerDict *myFlowerDict;

                static cFlower *ConstructModelFlower(int flower_type_index);
            };
        }
    }
}

namespace raven
{
    namespace sim
    {
        namespace gui
        {

            /**

            The base class for all objects that appear

            It is called a flower because it was designed
            for the GUI of a process flow simulator

            */
            class cFlower
            {

            public:
                /// A paramater
                struct sparam
                {
                    string name;        /// used for ID, no spaces no quotes
                    string description; /// advice to user, no quotes
                    double value;       /// value of the parmater
                };
                /// The parameters for this flower
                std::map<std::string, sparam> myParam;
                void AddParam(
                    const string &name,
                    const string &description = "",
                    double defaultvalue = 0.0);
                double getValue(
                    const string &name) const;

                cFlower();
                virtual ~cFlower() {}

#ifdef WXWIDGETS
                virtual void Paint(wxPaintDC &dc);
                void Set(wxPaintDC &dc);
                void PaintPipe(wxPaintDC &dc);
                void PaintArrow(wxPaintDC &dc, wxPoint exit_port, wxPoint entry_port);
                void DrawText(wxPaintDC &dc);
                void PaintResizeHandles(wxPaintDC &dc);
                virtual void Configure(wxWindow *parent);
                int HandleIsUnder(const wxPoint &p);
                void getHandles(std::vector<wxRect> &vhandle);
                void DragHandle(int handle, wxPoint p);
                void getLocation(wxPoint &p)
                {
                    p.x = myX;
                    p.y = myY;
                }
                virtual wxPoint getEntryPort();
                virtual void AppendProperties(wxPropertyGrid *pg);
                virtual void getPortLocation(int &x, int &y, const cFlower &other);

#endif
                virtual void locationExitPort1( int &x, int& y)
                {
                    x = myX+50;
                    y = myY+20;
                }
                virtual void locationExitPort2( int &x, int& y)
                {
                    x = myX+50;
                    y = myY+40;
                }
                void setSize(int w, int h)
                {
                    myWidth = w;
                    myHeight = h;
                }
                virtual void setLocationTopLeft(int x, int y);
                virtual void setLocationCenter(int x, int y);
                bool IsUnder(int x, int y);
                int getIndex()
                {
                    return myIndex;
                }
                void setIndex(int id);
                void Select()
                {
                    IsSelected = true;
                }
                void Deselect()
                {
                    IsSelected = false;
                }
                bool isSelected()
                {
                    return IsSelected;
                }
                void setName(const string &n = "");
                const string &getName() const
                {
                    return myName;
                }
                string &getTypeName()
                {
                    return myTypeName;
                }
                int getType() const
                {
                    return myType;
                }
                void Write(FILE *fp);
                void WritePipe(FILE *fp);
                void Read(const string &line);

                /// Connect to destination
                /// @param d 
                /// if port1 used, use port2
                void Connect(cFlower *d);

                /// @brief connect to destination using port2
                /// @param d 
                void Connect2(cFlower *d)
                {
                    myDestination2 = d; ///< Connect output to a destination flower
                }
                cFlower *getDestination()
                {
                    return myDestination; ///< Flower connected to exit port
                }
                cFlower *getDestination2()
                {
                    return myDestination2; ///< Flower connected to exit port
                }
                int getOutPortCount()
                {
                    return myOutputPortCount;
                }
                int getLocationX() const
                {
                    return myX;
                }
                int getLocationY() const
                {
                    return myY;
                }
                int getWidth()
                {
                    return myWidth;
                }
                int getHeight()
                {
                    return myHeight;
                }

            protected:
                string myName;
                int myType;
                string myTypeName;
                string myShape;
                int myX;
                int myY;
                int myWidth;
                int myHeight;
                int myIndex;
                static int lastIndex;
                bool IsSelected;
                cFlower *myDestination;
                cFlower *myDestination2;
                int myOutputPortCount;
            };

            class cPipeBend : public cFlower
            {
            public:
                cPipeBend();
#ifdef WXWIDGETS
                void Paint(wxPaintDC &dc);
                wxPoint getEntryPort();
                wxPoint getExitPort();
                void getPortLocation(int &x, int &y, const cFlower &other);
                void setLocation(wxPoint p);
#endif
            };

        }
    }
}
