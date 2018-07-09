const express = require('express');
const router = express.Router();
const User = require('../models/User');
 
router.route('/signup')
  .post(function (req, res, next){
    try {
      User.create(req.body , function(err , user){
        if(err) return res.json({errors:err.errors , page:1 , user:req.body});
        res.json({email:[user.email]})
      })
    } catch(error) {
      next(error)
    }
  })
  

module.exports = router;