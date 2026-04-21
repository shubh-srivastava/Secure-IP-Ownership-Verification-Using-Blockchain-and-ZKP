import { useState } from "react";
import { motion } from "framer-motion";
import { UserPlus, Key } from "lucide-react";
import { createUserKeys } from "../services/zkp";

const pageVariants = {
  initial: { opacity: 0, y: 20 },
  animate: { opacity: 1, y: 0, transition: { duration: 0.4 } },
  exit: { opacity: 0, y: -20, transition: { duration: 0.3 } }
};

export default function RegisterUser() {
  const [userID, setUserID] = useState("");
  const [msg, setMsg] = useState("");
  const [isSuccess, setIsSuccess] = useState(false);

  async function submit(e) {
    e.preventDefault();
    const keys = createUserKeys(userID);

    try {
      const res = await fetch("http://localhost:18080/register", {
        method: "POST",
        headers: { "Content-Type": "application/json" },
        body: JSON.stringify({ userID, publicKey: keys.pub })
      });
      const data = await res.json();

      if (data.success) {
        setIsSuccess(true);
        setMsg(`✅ Registered. Public Key: ${keys.pub}`);
        setUserID("");
      } else {
        setIsSuccess(false);
        setMsg("❌ User already exists");
      }
    } catch {
      setIsSuccess(false);
      setMsg("❌ Server connection failed");
    }
  }

  return (
    <motion.div variants={pageVariants} initial="initial" animate="animate" exit="exit" className="max-w-md mx-auto mt-12">
      <div className="glass-panel p-8">
        <div className="flex justify-center mb-6">
          <div className="p-4 bg-purple-500/20 rounded-full">
            <UserPlus className="w-10 h-10 text-purple-400" />
          </div>
        </div>
        
        <h2 className="text-3xl font-bold text-center mb-2 text-white">Register User</h2>
        <p className="text-slate-400 text-center mb-8">Generate a cryptographic identity</p>

        <form onSubmit={submit} className="space-y-5">
          <div>
            <label className="block text-sm font-medium text-slate-300 mb-1">User ID</label>
            <input
              className="input-field"
              value={userID}
              onChange={e => setUserID(e.target.value)}
              placeholder="e.g., alice_dev"
              required
            />
          </div>
          <button className="btn-primary w-full flex items-center justify-center gap-2">
            <Key className="w-5 h-5" />
            Generate & Register
          </button>
        </form>

        {msg && (
          <motion.div initial={{ opacity: 0, y: 10 }} animate={{ opacity: 1, y: 0 }} className={`mt-6 p-4 rounded-xl border ${isSuccess ? 'bg-green-500/10 border-green-500/20 text-green-400' : 'bg-red-500/10 border-red-500/20 text-red-400'}`}>
            <p className="text-sm font-medium break-all">{msg}</p>
          </motion.div>
        )}
      </div>
    </motion.div>
  );
}
