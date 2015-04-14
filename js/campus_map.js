(function($) {
	Drupal.campusMap = function() {
    var map;
    var MY_MAPTYPE_ID = 'custom_style';
    var featureOpts = [
        {
          "stylers": [
            { "visibility": "off" },
          ]
        },
        {
          "featureType": "road",
          "stylers": [
            { "visibility": "on" }
          ]
        },
        {
          "featureType": "landscape",
          "stylers": [
            { "visibility": "on" }
          ]
        },
        {
          "featureType": "water",
          "stylers": [
            { "visibility": "on" }
          ]
        },
        {
          "featureType": "landscape.man_made",
          "stylers": [
            { "visibility": "off" }
          ]
        }
      ];

      var mapOptions = {
        zoom: 15,
        center: new google.maps.LatLng(41.660070, -91.538403),
        mapTypeId: MY_MAPTYPE_ID,
        disableDefaultUI: true
      };
      map = new google.maps.Map(document.getElementById('campus_map'), mapOptions);

      var styledMapOptions = {
        name: 'Custom Style'
      };

      var customMapType = new google.maps.StyledMapType(featureOpts, styledMapOptions);

      map.mapTypes.set(MY_MAPTYPE_ID, customMapType);
    
    jQuery.get("http://data.its.uiowa.edu/maps/arc-buildings",
        function(data){
          jQuery.each(Drupal.settings.buildings, function(index,value){
            var buildingFillColor = '#FFE100';
            var buildingBorderColor = '#101010';
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
                  destpoints.push(new google.maps.LatLng(destproj[i].y, destproj[i].x));
                }
                building = new google.maps.Polygon({
                  paths: destpoints,
                  strokeColor: '#000000',
                  strokeWeight: 1,
                  fillColor: '#FEE100',
                  fillOpacity: 0.6
                });
                building.setMap(map);

                google.maps.event.addListener(building, 'click', function(event){
                  infoWindow = new google.maps.InfoWindow();
                  infoWindow.setContent('<p style="padding: 10px; margin: 0;"><strong><a href="http://maps.uiowa.edu/">'+value+'</a></strong></p>');
                  infoWindow.setPosition(event.latLng);
                  infoWindow.open(map);
                });
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