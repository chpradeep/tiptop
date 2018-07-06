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
                return res.json({status:true , versions:Object.keys(dev.Versions)});
            })
        }
    }
    else
        res.json({"Test":"fsdf"});
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

/* Register a Device. */
router.post('/register', function(req, res, next) {
    if(!req.body.TYPE)
        return res.json({'status':false , "message":"Device Type is missing" })
    Device.add(req.body , function(err , ans){
        if(err) return res.json({'status':false , "message":err })
        else return res.json(ans);
    });
});


router.post('/action' , function(req, res, next) {
    Device.act(req.body , function(err , ans){
        if(err) return res.json({'status':false , "message":err })
        else return res.json(ans);
    });
})

router.post('/event' , function(req, res, next) {
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

module.exports = router;
