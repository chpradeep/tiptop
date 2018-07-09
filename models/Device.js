const mongoose = require('mongoose');

const User = require("./User");

const Action = mongoose.model('Action' , new mongoose.Schema({
    time : {type:Date , default:Date.now()},
    type : {type:String , required:true}
    },{
    discriminatorKey: 'type', 
    collection: 'Actions'})
);

const Event = mongoose.model('Event' , new mongoose.Schema({
    time : {type:Date , default:Date.now()},
    type : {type:String , required:true}
    },{
    discriminatorKey: 'type', 
    collection: 'Events'})
);

const VersionSchema = new mongoose.Schema({
    Properties: Object,
    Actions: Object,
    Events: Object,
    date: {
        type: Number,
        default: new Date()
    },
    deprecated: {
        type: Boolean,
        default: false
    }
})

const Versions = mongoose.model('Versions', VersionSchema);

// Defining the schema for DeviceType
const DeviceTypeSchema = mongoose.Schema({
    Name: {
        type: String,
        required: true,
        unique:true
    },
    LatestVersion: {
        type: String,
        default : "v0.0"
    }
    ,Versions: Object
});

const DeviceType = mongoose.model('DeviceTypes', DeviceTypeSchema);

const Device = mongoose.model('Devices' , new mongoose.Schema({
	createdAt : {type:Date , default:Date.now()},
    TYPE : {type:String , required:true},
    lifeline : [{kind:{type:String,required:true} , id: {type:mongoose.Schema.Types.ObjectId ,required:true, refPath: 'lifeline.kind'},_id:false}]
    },{
    discriminatorKey: 'TYPE', 
    collection: 'Devices'}
));

module.exports.getAll = function(callback){
    DeviceType.find({},{Name:1,LatestVersion:1,_id:0},function(err,all){
        if(err) return callback("DB Error in fetching Device Types");
        return callback(all);
    })
}

module.exports.getByName = function(Name , callback){
    DeviceType.findOne({Name:Name} , callback)
}

module.exports.getByVersion = function (Name , ver, callback){
    DeviceType.findOne({Name:Name} , function (err,dev) {
        if(err) return callback(err,dev)
        if(dev && dev.Versions[ver]){
            Versions.findById(dev.Versions[ver] , function(err , unit){
                if(err) return callback(err,unit)
                return callback(null , Object.keys(unit.Actions));
            })
        }
    })
}

module.exports.actionAPI = function (Name , ver, actItem , callback){
    DeviceType.findOne({Name:Name} , function (err,dev) {
        if(err) return callback(err,dev)
        if(dev && dev.Versions[ver]){
            Versions.findById(dev.Versions[ver] , function(err , unit){
                if(err) return callback(err,unit)
                if(!unit.Actions[actItem])
                    return callback("Invalid Action "+actItem , null);
                
                else{
                    var result = unit.Actions[actItem];
                    result["CallbackEvents"] = {}
                    if(result.Events){
                        result.Events.forEach(function(eve , index , arr){
                            result.CallbackEvents[eve] = unit.Events[eve];
                            if(index == arr.length-1){
                                delete(result.Events);
                                return callback(null , result)
                            }
                        });                    
                    }
                    else
                        return callback(null , result);
                }
            })
        }
    })
}

DeviceType.find({},function(err , devs){
    if(err) return;
    devs.forEach(function(dev , index, arr){
        var Name = dev.Name;
        for(var ver in dev.Versions){
            Versions.findById(dev.Versions[ver] , function(err , unit){
                if (err) return console.log(err , unit);
                console.log(Name+ver);
                Device.discriminator( Name+ver , new mongoose.Schema(unit.Properties));
                for(var act in unit.Actions){
                    delete(unit.Actions[act].Events);
                    Action.discriminator( Name+ver+act , new mongoose.Schema(unit.Actions[act]));
                }
                for(var eve in unit.Events)
                    Event.discriminator(Name+ver+eve , new mongoose.Schema(unit.Events[eve]));
            });
        }
    })
})

function validate(input, callback){
    var result = {Properties:{},Actions:{},Events:{}};
    try{
        result['Properties'] = new mongoose.Schema(input.Properties);
        for(var act in input.Actions){
            var callbackEvents = false;
            if(input.Actions[act]['Events']){
                callbackEvents = input.Actions[act]['Events'];
                delete(input.Actions[act]['Events']);
            }
            for(var p in input.Actions[act]){
                if(input.Actions[act][p] === 'Property'){
                    if(!input.Properties[p])
                        return callback( "Undefined Property "+p  , false , false);
                    input.Actions[act][p] = input.Properties[p];
                }
            }
            result['Actions'][act] = new mongoose.Schema(input.Actions[act]);
            if(callbackEvents)
                input.Actions[act]['Events'] = callbackEvents;
        }
        for(var eve in input.Events){
            for(var p in input.Events[eve]){
                if(input.Events[eve][p] === 'Property'){
                    if(!input.Properties[p])
                        return callback("Undefined Property "+p , false , false);
                    input.Events[eve][p] = input.Properties[p];
                }
            }
            result['Events'][eve] = new mongoose.Schema(input.Events[eve]);
        }
        callback(null , input , result)
    }
    //console.log(Sc);
    catch(e){
        if(e instanceof TypeError){
            console.log(e)
            return callback("Type Error in DeviceType" , null , null);
        }
        console.log(e);
        callback("Invalid Properties schema" , null , null);
    }
}

module.exports.add = function(dev , callback){
    User.findOne({'email':dev.UserKey},function (err , user) {
        if(err) return callback('DB error' , null);
        if(!user) return callback('User not found',null);
        else{
            Device.create(dev , function(err,device){
                if(err) return callback("Device creation error" , null);
                User.findByIdAndUpdate(user._id , {$push: {devices : device._id}}, function(err,dev){
                    if(err) return callback('User device list updation error' , null)
                    callback(null , device)
                });
            }) 
        }
    })
}

module.exports.create = function(input , callback){
    var DevTyp = {Versions:{}};
    DevTyp['Name'] = input.Name;
    var Vers = "v0.0";
    var orgInput = input;

    validate(orgInput , function(err , input , result){
        if(err) return callback(err,false);
        callback(false , input);
        Versions.create(input , function(err , ver){
            if(err) return console.log(err)
            DevTyp['Versions'][Vers] = ver._id
            console.log(DevTyp);
            DeviceType.create(DevTyp , function(err, res){
                console.log(err , res);
                Device.discriminator( res.Name+res.LatestVersion , result.Properties);
                for(var act in result.Actions)
                    Action.discriminator(res.Name+res.LatestVersion+act , result.Actions[act])
                for(var eve in result.Events)
                    Event.discriminator(res.Name+res.LatestVersion+eve , result.Events[eve])
            })
        })
    })

}

module.exports.act = function(action , callback){
    Device.findById(action.deviceId , function(err, dev){
        if(err) callback('Device finding DB error' , null)
        Action.create(action , function(err , act){
            if(err) callback('Action creating DB error' , null)
            Device.findByIdAndUpdate(dev._id , {$push: {lifeline : {kind:"Action" , id:act._id}}},function(err , dev){
                if(err) return callback("Device Action updation error" , null);
                return callback(null, act._id);
            })
        });  
    })
}

module.exports.getDeviceStatus = function(deviceId , callback){
    Device.findById(deviceId , {lifeline:0}, function(err, unit) {
        if(err) return callback("error in finding deviceId"+deviceId , null);
        if(!unit) return callback("Device not found with id "+deviceId , null);
        return callback(null , unit);
    })
}

module.exports.getDeviceHistory = function(deviceId , callback){
    Device.
        findById(deviceId , {lifeline:1,_id:0}).
        populate('lifeline.id').
        exec(function(err, unit) {
            if(err) return callback("error in finding deviceId"+deviceId+","+err , null);
            if(!unit) return callback("Device not found with id "+deviceId , null);
            return callback(null , unit);
        })
}
