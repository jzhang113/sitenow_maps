(function($) {
	Drupal.individualBuilding = function() {
    
    //Geo translation for the ArcGIS data
    Proj4js.defs["EPSG:3418"] = "+title=Iowa South (ft US) +proj=lcc +lat_1=41.78333333333333 +lat_2=40.61666666666667 +lat_0=40 +lon_0=-93.5 +x_0=500000.00001016 +y_0=0 +ellps=GRS80 +datum=NAD83 +to_meter=0.3048006096012192 +no_defs  " 

    //Generates the Mapbox map
		var map = L.mapbox.map('building_map_'+Drupal.settings.abbr)
      .addLayer(L.mapbox.tileLayer('uiowa-its.map-6ve6jxun', {
        	detectRetina: true
      }));

    //New layer for the accessible entrances
    var accessibleLayer = new L.layerGroup();

    jQuery.get("http://data.its.uiowa.edu/maps/arc-buildings",
          function(data){
            var buildingFillColor = '#FEE000';
            var buildingBorderColor = '#333333';
            map.setView([Drupal.settings.latitude[0],Drupal.settings.longitude[0]], 17);
            if(Drupal.settings.other_buildings){
              $.each(Drupal.settings.all_buildings, function(index,value){
                var arcdata = jQuery.grep(data.features, function(e){ return e.attributes.BLDABBR == index});
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
                    L.polygon(destpoints,
                    {   color: buildingBorderColor,
                        fillColor: buildingFillColor,
                        fillOpacity: 0.5,
                        weight: 1
                    }).addTo(map).bindPopup('<a href="http://maps.uiowa.edu/'+index.toLowerCase()+'">'+value+'</a>');
                }
              });
            }
            else{
              var arcdata = jQuery.grep(data.features, function(e){ return e.attributes.BLDABBR == Drupal.settings.abbr});
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
                  L.polygon(destpoints,
                  {   color: buildingBorderColor,
                      fillColor: buildingFillColor,
                      fillOpacity: 0.5,
                      weight: 1
                  }).addTo(map).bindPopup('<a href="http://maps.uiowa.edu/'+Drupal.settings.abbr+'">'+arcdata[0].attributes.BLDGNAME+'</a>');;
              }
              map.setView([arcdata[0].attributes.Lati,arcdata[0].attributes.Longi], 17);
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
                map.markerLayer.setGeoJSON(geoJson);        
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

	}

	Drupal.behaviors.individualBuilding = {
    attach: function(context, settings) {
      $('.building_panel_'+settings.abbr, context).once('individualBuilding', function() {
        Drupal.individualBuilding();
      });
    }
  };
})(jQuery);


