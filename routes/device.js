var express = require('express');
var router = express.Router();
var Device = require("../models/Device");
const fs = require( 'fs' );
const mongoose = require('mongoose');

router.get('/' , function(req, res , next) {
    if(req.query.type){
        if(req.query.version){
            if(req.query.actionAPI){
                Device.actionAPI(req.query.type , req.query.version , req.query.actionAPI , function(err,ActionAPI){
                    if(err) return res.json({status:false , message: "Invalid query"})
                    return res.json({status:true , ActionAPI: ActionAPI})
                });
            }
            else
                Device.getByVersion(req.query.type , req.query.version , function(err,actionsList){
                    if(err) return res.json({status:false , message: "Invalid query"})
                    return res.json({status:true , actionsList: actionsList})
                });
        }
        else{
            console.log(req.query.type)
            Device.getByName(req.query.type , function(err , dev){
                if(err) return res.json({status:false , message: "Mongo error"})
                var temp = [];
                Object.keys(dev.Versions).forEach(function(Mver , Mindex, Marr){
                    Object.keys(dev.Versions[Mver]).forEach(function(mver , mindex, marr){
                        temp.push(Mver+"."+mver);
                        if(Mindex == Marr.length-1 && mindex == marr.length-1)
                            return res.json({status:true , versions:temp});
                    })
                })
            })
        }
    }
    else
        Device.getAll(function(err , dev){
            if(err) return res.json({status:false , message: err})
            return res.json({status:true , DeviceTypes:dev});
        })
})

/* Crate DeviceType. */
router.post('/', function(req, res, next) {
    if(!req.body.Name)
        return res.json({'status':false , "message":"Device Name is missing" })
    Device.create(req.body , function(err , ans){
        if(err) return res.json({'status':false , "message":err })
        else return res.json(ans);
    });
});

/* Update DeviceType. */
router.put('/:Type/:changeType', function(req, res, next) {
    if(!req.params.Type)
        return res.json({'status':false , "message":"Device Name is missing" })
    if(req.params.changeType && (req.params.changeType.toLowerCase()=== 'major' || req.params.changeType.toLowerCase()=== 'minor' )){
        Device.modify(req.params.Type , req.params.changeType , 
        req.body , function(err , ans){
            if(err) return res.json({'status':false , "message":err })
            else return res.json(ans);
        });
    }
    else
        return res.json({'status':false , "message":"Please mention the updation type as major or minor" })
});

router.delete('/:type/:version' , function(req , res){
  if(req.params.type && req.params.version){
    Device.delete(req.params.type , req.params.version , function(err , ans){
     if (err)
          res.json({
              success: false,
              msg: err
          });
        else
          res.json({
              success: true,
              msg: ans
          }); 
    });
  }
  else
    res.json({success:false , msg:'Invalid params'})
})


/* Register a Device. */
router.post('/register', function(req, res, next) {
    if(!req.body.TYPE)
        return res.json({'status':false , "message":"Device Type is missing" })
    if(!req.body.UserKey)
        return res.json({'status':false , "message":"UserKer is missing" })
    if(!req.body.MAC)
        return res.json({'status':false , "message":"Device MAC is missing" })
    Device.add(req.body , function(err , ans){
        if(err) return res.json({'status':false , "message":err })
        else return res.json(ans);
    });
});


router.post('/action' , function(req, res, next) {
    if(!req.body.deviceId)
        return res.json({status:false , message: "Please provide a device ID"})
    Device.act(req.body , function(err , ans){
        if(err) return res.json({'status':false , "message":err })
        else return res.json(ans);
    });
})

router.post('/event' , function(req, res, next) {
    if(!req.body.deviceId)
        return res.json({status:false , message: "Please provide a device ID"})
    Device.eve(req.body , function(err , ans){
        if(err) return res.json({'status':false , "message":err })
        else return res.json(ans);
    });
})

/* update devivce firmware */
router.get( '/update/:filename', function( req, res ) {
    console.log('Firmware request received' );
    console.log(req.params);
    fs.readFile( `firmware/${ req.params.filename }`, function( err, data ) {
        if( err ) {
            if( err.code === 'ENOENT' ) {
                return res.json({'error': 'firmware file not found' } );
            }
            console.log( 'Failed to read firmware file' );
            return res.json( {'error' : 'restify.InternalServerError'});
        }
        res.setHeader( 'Content-Type', 'application/octet-stream' );
        res.setHeader( 'Content-Disposition', 'attachment; filename=update.bin' );
        res.setHeader( 'Content-Length', data.length );
        res.writeHead( 200 );
        res.end( data );
    } );
} );

router.get('/:deviceId/status' , function(req, res) {
    console.log(req.params.deviceId);
    Device.getDeviceStatus(req.params.deviceId , function (err,dev) {
        if(err) return res.json({'status':false , "message":err })
        else return res.json(dev);
    })
})

router.get('/:deviceId/history' , function(req, res) {
    console.log(req.params.deviceId);
    Device.getDeviceHistory(req.params.deviceId , function (err,dev) {
        if(err) return res.json({'status':false , "message":err })
        else return res.json(dev);
    })
})

module.exports = router;
