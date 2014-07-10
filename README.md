# University of Iowa Map Panes

This module allows you to add a panel pane of either a campus map or an individual building. Built using the CTools Content Type, each type of pane has some configuration options.

The maps are built using the [Mapbox.js](https://www.mapbox.com/mapbox.js/) library with the buildings added using data from the Facilities ArcGIS server.

## Campus Map

The campus map shows all of the buildings on campus and is centered on the river. Additional features coming soon include:

* Additional layer toggles
* Defining the zoom level
* Defining a new center of map

## Individual Building

The individual building map allows you to pick a specific building to detail. You can show just that building or all buildings on campus, with the map centered on that building. You can also bring over the street address of the building through an option in the pane. Another option is whether you want to display accessable entrances on the map or not, as well as the map height.

You can also dynamically set which building to show. To do this, set the Building - Advanced Options Building field to a panels context keyword of your choice on the pane configuration.

## Map Point
The map point pane allows you to set a latitude and longitude point to render on
the map. You can optionally set popup text when the point is clicked.
Additionally, you can set the many of the other options that are available in
the individual builing pane.
