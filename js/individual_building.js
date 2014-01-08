(function($) {
	Drupal.maps = function() {
    Proj4js.defs["EPSG:3418"] = "+title=Iowa South (ft US) +proj=lcc +lat_1=41.78333333333333 +lat_2=40.61666666666667 +lat_0=40 +lon_0=-93.5 +x_0=500000.00001016 +y_0=0 +ellps=GRS80 +datum=NAD83 +to_meter=0.3048006096012192 +no_defs  " 

		var map = L.mapbox.map('building_map')
		
		.addLayer(L.mapbox.tileLayer('uiowa-its.map-6ve6jxun', {
        	detectRetina: true
    	}));

    jQuery.get("https://maps.facilities.uiowa.edu/arcgis/rest/services/Base/BaseMap/FeatureServer/0/query?where=BLDABBR+like+%27%%27&outFields=BLDABBR,BLDGNAME,Lati,Longi&f=pjson",
          function(data){
              var buildingFillColor = '#FEE000';
              var buildingBorderColor = '#333333';
              var arcdata = jQuery.grep(data.features, function(e){ return e.attributes.BLDABBR == Drupal.settings.abbr});
              var destpoints = new Array();
              var destproj = new Array();
              var sourcepoints = new Array();
              console.log();
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
              map.setView([arcdata[0].attributes.Lati,arcdata[0].attributes.Longi], 17)
              
          }, "json"
      );

	}

	Drupal.behaviors.maps = {
    attach: function(context, settings) {
      $('.pane-individual-building', context).once('maps', function() {
        Drupal.maps();
      });
    }
  };
})(jQuery);


