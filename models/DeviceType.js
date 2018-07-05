const mongoose = require('mongoose');

// Defining the schema for DeviceType
const DeviceTypeSchema = mongoose.Schema({
    Nam: {
        type: String,
        required: true,
        unique:true
    },
    Ltv: {
        type: String,
        default : "v0.0"
    },
    Ver: [
        new mongoose.Schema({
            Vno:{
                type: String,
                required: true,
                default : "v0.0"
            },
            Pro: [new mongoose.Schema({
                Nam: {
                    type: String,
                    required: true
                },
                Dtp: {
                    type: String,
                    enum: ["integer", "string", "boolean"],
                    required: true
                },
                Ptp: {
                    type: String,
                    enum:  ["SEN" , "ACT" , "MTA" , "CFG"],
                    required: true
                }
            })],
            Act: [new mongoose.Schema({
                Nam: {
                    type: String,
                    required: true
                },
                
                fields: [{
                    type: String
                }]
            })],
            events: [new mongoose.Schema({
                type: {
                    type: String,
                    required: true
                },
                fields: [{
                    type: String
                }]
            })],
            date: {
                type: Number,
                default: new Date()
            },
            deprecated: {
                type: Boolean,
                default: false
            }
        })
    ]

});