var express = require('express');
var router = express.Router();
var Device = require("../models/Device");
const fs = require( 'fs' );
const mongoose = require('mongoose');

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
