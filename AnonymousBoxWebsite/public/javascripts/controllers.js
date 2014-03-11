function MessagesCtrl($scope, $http){
    $scope.glitch = false;
    $glitcharea = $("#glitcharea");
    $http.get('/getmessages').
        success(function(data,status,headers,config){
            console.log("got messages: ", data);
            $scope.messages = data;
            glitch();
        });
    var glitch = function(){
        var sglitch = $scope.glitch
        $scope.$apply(function(){
            $scope.glitch = !$scope.glitch;
            console.log("glitch is ", $scope.glitch);
        });
        if(!sglitch){
            setTimeout(glitch, Math.floor(Math.random()*3000)+200);
            $glitcharea.toggleClass("glitch");
        }else{
            setTimeout(glitch, Math.floor(Math.random()*10000)+1);
        }
                    
    }
}
