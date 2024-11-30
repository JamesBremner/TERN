/*
  Interfaces for proccess flow simulation GUI classes

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
#include <vector>

#include "flower.h"

namespace raven
{
namespace sim
{
namespace gui
{

/**

	Container for the flowers

	*/
class cVase
{
public:

    enum e_type
    {
        none,
        task,
        flow
    } mySimType;

    cVase();

    bool	Add(  const std::string& flower_type_name );
    bool    Add( int flower_type_index );
    bool	Read( const std::string& filename );
    bool	Write( const std::string& filename );

    void    DBWrite();

    /// @brief Read slected flower plot data from database
    void readPlot();
    const std::vector<double>& getPlot() const
    {
        return myPlotData;
    }

    bool	IsSelected()
    {
        return ( mySelected != NULL );
    }
    cFlower * getSelected()
    {
        return mySelected;
    }
    void setSelected( cFlower* f );

    cFlower * getSelected2()
    {
        return my2Selected;
    }


    void	Delete();
    void	setName( const string& n = "" );
    bool	AllNamesUnique();
    void	Connect();
    void    Connect2();
    void	Clear();

    int     getFlowerCount()
    {
        return (int) myVase.size();
    }
    typedef std::vector < cFlower* >::iterator iterator;
    iterator begin()
    {
        return myVase.begin();
    }
    iterator end()
    {
        return myVase.end();
    }

    void setSimTime( int t )    { mySimTime = t; }
    int getSimTime()            { return mySimTime; }
    void setPlotPoints( int n ) { myPlotPoints = n; }
    int getPlotPoints()         { return myPlotPoints; }

    cFlower*  find( const std::string& name );
    cFlower* find( int x, int y );

private:
    std::vector < cFlower* > myVase;
    cFlower * mySelected;
    cFlower * my2Selected;

    int mySimTime;
    int myPlotPoints;
    std::vector<double> myPlotData;

    iterator  find( int idx );

};

}
}
}
