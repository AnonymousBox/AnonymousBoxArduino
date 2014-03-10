function MessagesCtrl($scope, $http){
    $scope.glitch = false;
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
            setTimeout(glitch, Math.floor(Math.random()*300)+200);
        }else{
            setTimeout(glitch, Math.floor(Math.random()*10000)+1);
        }
                    
    }
}
