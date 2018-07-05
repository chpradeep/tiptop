var express = require('express');
var router = express.Router();
var DeviceType = require("../models/DeviceType");
const fs = require( 'fs' );

/* Crate DeviceType. */
router.post('/', function(req, res, next) {
  res.send('respond with a resource');
});

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
