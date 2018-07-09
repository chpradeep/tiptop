const mongoose = require('mongoose')
const Schema = mongoose.Schema
const bcrypt =require('bcryptjs')

const userSchema = new Schema({
   Name : {type: String , required:[{message:"Name is empty",code:1}] },
   email: {type: String,trim: true, required: {message:'Email is empty',code:2}, unique:true, match: [/^\w+([\.-]?\w+)*@\w+([\.-]?\w+)*(\.\w{2,3})+$/, 'Invalid email address']},
   Password: {
   	type: String, 
   	required: {message:'Password is empty',code:3} , 
		//min:[6 , 'Password should contain min 6 characters']
		validate: [function(pwd){
			return (pwd.length>5)
		},'Password should contain min 6 characters']
   },
   role:{type:String , required:true , enum:['user','admin'] , default:'user'},
   devices:[{type:mongoose.Schema.Types.ObjectId , ref:'Devices'}]
})

userSchema.methods.validPassword = function(password) {
    return bcrypt.compareSync(password, this.Password);
};

userSchema.pre('save',function (next) {
	bcrypt.genSalt(10 , (err,salt) =>{
	    if(err) throw err;
		bcrypt.hash(this.Password , salt , (err,hash) =>{
			if(err) return next(err);
			this.Password = hash;
			next();
		})
	})
})

var handleErrors = function(error, res, next) {
	//console.log(error)
  if(error.name === 'ValidationError'){
  	var errors = {};
  	var len = Object.keys(error.errors).length;
  	var cur = 1;
  	for(var k in error.errors){
  		errors[k] = error.errors[k].message;
  		if(cur == len) return next({'errors':errors})
  		cur++;
  	}
  }
  if (error.name === 'MongoError' && error.code === 11000)	
	return next({errors:{email:'Email already exist'}});
  next();
};

userSchema.post('save', handleErrors);

const User = module.exports = mongoose.model('user',userSchema)