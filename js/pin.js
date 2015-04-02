(function($) {
	Drupal.pin = function() {
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
        zoom: 17,
        center: new google.maps.LatLng(Drupal.settings.sitenowMaps.latitude, Drupal.settings.sitenowMaps.longitude),
        mapTypeId: MY_MAPTYPE_ID,
        disableDefaultUI: true
      };
      map = new google.maps.Map(document.getElementById('pin_map'), mapOptions);

      var styledMapOptions = {
        name: 'Custom Style'
      };

      var customMapType = new google.maps.StyledMapType(featureOpts, styledMapOptions);

      map.mapTypes.set(MY_MAPTYPE_ID, customMapType);



    jQuery.get("http://data.its.uiowa.edu/maps/arc-buildings",
          function(data){
          
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
                      destpoints.push(new google.maps.LatLng(destproj[i].y, destproj[i].x));
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
          
                    building = new google.maps.Polygon({
                      paths: destpoints,
                      strokeColor: buildingBorderColor,
                      strokeWeight: buildingLineWeight,
                      fillColor: buildingFillColor,
                      fillOpacity: buildingFillOpacity
                    });
                    building.setMap(map);
                    google.maps.event.addListener(building, 'click', function(event){
                      infoWindow = new google.maps.InfoWindow();
                      infoWindow.setContent('<p style="padding: 10px; margin: 0;"><strong><a href="'+Drupal.settings.basePath+index+'">'+value+'</a></strong></p>');
                      infoWindow.setPosition(event.latLng);
                      infoWindow.open(map);
                    });
                  }
              });
            }
            else{
              var arcdata = jQuery.grep(data.features, function(e){ return e.attributes.BuildingAbbreviation == Drupal.settings.abbr});
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
              }
            }
          }, "json"
      );

      var entrances = [];

      if(Drupal.settings.accessible_entrances){
        jQuery.get("http://data.its.uiowa.edu/maps/arc-accessible-entrances",
            function(data){
              for(var i = 0; i < data.features.length; i++){
                  var point = Proj4js.transform(new Proj4js.Proj('EPSG:3418'), new Proj4js.Proj('WGS84'), new Proj4js.Point([data.features[i].geometry.x,data.features[i].geometry.y] ));
                  var image = 'http://maps.uiowa.edu/files/maps.uiowa.edu/files/styles/large/public/disability.png';
                  var marker = new google.maps.Marker({
                    position: new google.maps.LatLng(point.y, point.x),
                    map: map,
                    icon: image
                  });
                  entrances.push(marker);
              }
              setAllMap(map);
            }, "json"
        );
      }

      google.maps.event.addListener(map, 'zoom_changed', function() {
        if(map.getZoom() >= 16){
          setAllMap(map);
        }
        else{
          setAllMap(null);
        }
      });

      function setAllMap(map){
        for (var i = 0; i < entrances.length; i++) {
          entrances[i].setMap(map);
        }
      }

    // Add custom marker.
    var customMarker = new google.maps.Marker({
      position: new google.maps.LatLng(Drupal.settings.sitenowMaps.latitude, Drupal.settings.sitenowMaps.longitude),
      map: map
    });

    // Set popup text for custom marker.
    if(Drupal.settings.sitenowMaps.popup_text) {
      google.maps.event.addListener(customMarker, 'click', function(event){
        infoWindow = new google.maps.InfoWindow();
        infoWindow.setContent(Drupal.settings.sitenowMaps.popup_text);
        infoWindow.setPosition(event.latLng);
        infoWindow.open(map);
      });
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


