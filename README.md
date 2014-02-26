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

You can also dynamically set which building to show. To do this, on your page, add a Token context to the panels. Then, if there is a field with the machine name of field_abbreviation, the pane will look for that and set the building code to that. You can use any of the building abbreviations then to display a building map!!!!
