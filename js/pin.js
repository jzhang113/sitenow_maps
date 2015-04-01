(function($) {
	Drupal.pin = function() {

    //Geo translation for the ArcGIS data.
    Proj4js.defs["EPSG:3418"] = "+title=Iowa South (ft US) +proj=lcc +lat_1=41.78333333333333 +lat_2=40.61666666666667 +lat_0=40 +lon_0=-93.5 +x_0=500000.00001016 +y_0=0 +ellps=GRS80 +datum=NAD83 +to_meter=0.3048006096012192 +no_defs  "

    //Generates the Mapbox map.
		var map = L.mapbox.map('pin_map')
      .addLayer(L.mapbox.tileLayer('uiowa-its.map-6ve6jxun', {
        	detectRetina: true
      }));

    //New layer for the accessible entrances.
    var accessibleLayer = new L.layerGroup();

    jQuery.get("http://data.its.uiowa.edu/maps/arc-buildings",
          function(data){

            map.setView([Drupal.settings.sitenowMaps.latitude,Drupal.settings.sitenowMaps.longitude], 17);
            if(Drupal.settings.all_buildings){
              $.each(Drupal.settings.all_buildings, function(index,value){
                var arcdata = jQuery.grep(data.features, function(e){ return e.attributes.BuildingAbbreviation == index});
                var destpoints = new Array();
                var destproj = new Array();
                var sourcepoints = new Array();
                if(arcdata[0]){
                  sourcepoints = arcdata[0].geometry.rings[0];
                    for(var i=0;i<sourcepoints.length;i++){
                        destproj.push(Proj4js.transform(new Proj4js.Proj('EPSG:3418'), new Proj4js.Proj('WGS84'), new Proj4js.Point(sourcepoints[i])));
                    }
                    for(var i=0;i<destproj.length;i++){
                        destpoints.push([destproj[i].y, destproj[i].x]);
                    }
                    if(Drupal.settings.abbr == index){
                      var buildingFillColor = '#FEE100';
                      var buildingBorderColor = '#000000';
                      var buildingFillOpacity = 0.8;
                      var buildingLineWeight = 2;
                    }
                    else{
                      var buildingFillColor = '#81642B';
                      var buildingBorderColor = '#333333';
                      var buildingFillOpacity = 0.5;
                      var buildingLineWeight = 1;
                    }
                    L.polygon(destpoints,
                    {   color: buildingBorderColor,
                        fillColor: buildingFillColor,
                        fillOpacity: buildingFillOpacity,
                        weight: buildingLineWeight
                    }).addTo(map).bindPopup('<a href="http://maps.uiowa.edu/'+index.toLowerCase()+'">'+value+'</a>');
                }
              });
            }
          }, "json"
      );


      if(Drupal.settings.accessible_entrances){
        jQuery.get("http://data.its.uiowa.edu/maps/arc-accessible-entrances",
          function(data){
              var geoJson = [];
              for(var i = 0; i < data.features.length; i++){
                  var point = Proj4js.transform(new Proj4js.Proj('EPSG:3418'), new Proj4js.Proj('WGS84'), new Proj4js.Point([data.features[i].geometry.x,data.features[i].geometry.y] ));
                  var marker = {
                    type:'Feature',
                    "geometry":{"type":"Point","coordinates": [point.x, point.y]},
                    "properties":{
                      'marker-symbol':'disability',
                      'marker-size':'small',
                      'marker-color':'#0074D9'
                    }
                  }
                  geoJson.push(marker);
              }
              map.featureLayer.setGeoJSON(geoJson);
          }, "json"
        );

        //Hides the accessible entrances on zoomed out levels
        map.on('zoomend', function(e){
          if(this.getZoom() >= 16){
              this.addLayer(map.markerLayer);
          }
          else{
              this.removeLayer(map.markerLayer);
          }
      });
    }

    // Add custom marker.
    var marker = L.marker([Drupal.settings.sitenowMaps.latitude,Drupal.settings.sitenowMaps.longitude]).addTo(map);

    // Set popup text for custom marker.
    if(Drupal.settings.sitenowMaps.popup_text) {
      marker.bindPopup(Drupal.settings.sitenowMaps.popup_text).openPopup();
    }
	}

	Drupal.behaviors.pin = {
    attach: function(context, settings) {
      console.log(context);
      console.log(settings);
      $('.building_panel_pin', context).once('pin', function() {
        Drupal.pin();
      });
    }
  };
})(jQuery);

