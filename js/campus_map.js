(function($) {
	Drupal.campusMap = function() {
    
    //Geo translation for the ArcGIS data
    Proj4js.defs["EPSG:3418"] = "+title=Iowa South (ft US) +proj=lcc +lat_1=41.78333333333333 +lat_2=40.61666666666667 +lat_0=40 +lon_0=-93.5 +x_0=500000.00001016 +y_0=0 +ellps=GRS80 +datum=NAD83 +to_meter=0.3048006096012192 +no_defs  " 

    //Generates the Mapbox map
		var map = L.mapbox.map('campus_map')
		  .addLayer(L.mapbox.tileLayer('uiowa-its.map-6ve6jxun', {detectRetina: true}))
      .setView([41.660070, -91.538403], 15);

     

    var blueCapsLayer = L.mapbox.featureLayer();

    L.control.layers(
        {},
        {
          'Campus Zones':L.mapbox.tileLayer('uiowa-its.kb2u4jhb'),
          'Code Blue Phones':blueCapsLayer
        }
    ).addTo(map)

    jQuery.get("http://data.its.uiowa.edu/maps/arc-bluecaps",
      function(data){
           
          var blueCaps = [];
          for(var i = 0; i < data.features.length; i++){
              var point = Proj4js.transform(new Proj4js.Proj('EPSG:3418'), new Proj4js.Proj('WGS84'), new Proj4js.Point([data.features[i].geometry.paths[0][0][0],data.features[i].geometry.paths[0][0][1]] ));
              var marker = {
                type:'Feature',
                "geometry":{"type":"Point","coordinates": [point.x, point.y]},
                "properties":{
                  title:data.features[i].attributes.Location,
                  'marker-symbol':'emergency-telephone',
                  'marker-size':'small',
                  'marker-color':'#0074D9'
                }
              }
              blueCaps.push(marker);
          }
          blueCapsLayer.setGeoJSON(blueCaps);
      }, "json");

    

    //Disables the scroll wheel, will make an option later on
    map.scrollWheelZoom.disable();
    
    jQuery.get("http://data.its.uiowa.edu/maps/arc-buildings",
        function(data){
          jQuery.each(Drupal.settings.buildings, function(index,value){
            var buildingFillColor = '#FFE100';
            var buildingBorderColor = '#101010';
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
        }, "json"
    );
	}

	Drupal.behaviors.campusMap = {
    attach: function(context, settings) {
      $('.pane-campus-map', context).once('campusMap', function() {
        Drupal.campusMap();
      });
    }
  };
})(jQuery);