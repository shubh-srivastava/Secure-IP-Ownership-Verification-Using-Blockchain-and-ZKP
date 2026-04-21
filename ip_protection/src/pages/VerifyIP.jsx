import { useState } from "react";
import { motion } from "framer-motion";
import { Fingerprint, CheckCircle2, XCircle } from "lucide-react";
import { generateProof, getKeys } from "../services/zkp";

const pageVariants = {
  initial: { opacity: 0, y: 20 },
  animate: { opacity: 1, y: 0, transition: { duration: 0.4 } },
  exit: { opacity: 0, y: -20, transition: { duration: 0.3 } }
};

export default function VerifyIP() {
  const [creator, setCreator] = useState("");
  const [content, setContent] = useState("");
  const [msg, setMsg] = useState("");
  const [isSuccess, setIsSuccess] = useState(null);

  async function submit(e) {
    e.preventDefault();

    const keys = getKeys(creator);
    if (!keys) {
      setIsSuccess(false);
      setMsg("No local key for this user. Register first.");
      return;
    }

    const proof = generateProof(creator, content);
    if (!proof) {
      setIsSuccess(false);
      setMsg("Proof generation failed. Ensure inputs are correct.");
      return;
    }

    try {
      const res = await fetch("http://localhost:18080/verify", {
        method: "POST",
        headers: { "Content-Type": "application/json" },
        body: JSON.stringify({
          creator,
          content,
          commitment: proof.commitment,
          response: proof.response
        })
      });

      const data = await res.json();
      setIsSuccess(data.valid);
      setMsg(data.valid ? "ZKP Verified! Ownership confirmed." : "Verification failed. Incorrect data.");
      
      if(data.valid) {
        setCreator("");
        setContent("");
      }
    } catch {
      setIsSuccess(false);
      setMsg("Server connection failed");
    }
  }

  return (
    <motion.div variants={pageVariants} initial="initial" animate="animate" exit="exit" className="max-w-lg mx-auto mt-12">
      <div className="glass-panel p-8 relative overflow-hidden">
        {/* Subtle decorative glow */}
        {isSuccess === true && <div className="absolute inset-0 bg-green-500/10 blur-3xl pointer-events-none" />}
        {isSuccess === false && <div className="absolute inset-0 bg-red-500/10 blur-3xl pointer-events-none" />}

        <div className="flex justify-center mb-6 relative z-10">
          <div className="p-4 bg-emerald-500/20 rounded-full">
            <Fingerprint className="w-10 h-10 text-emerald-400" />
          </div>
        </div>
        
        <h2 className="text-3xl font-bold text-center mb-2 text-white relative z-10">Verify Ownership</h2>
        <p className="text-slate-400 text-center mb-8 relative z-10">Zero-Knowledge Proof Verification</p>

        <form onSubmit={submit} className="space-y-5 relative z-10">
          <div>
            <label className="block text-sm font-medium text-slate-300 mb-1">Creator ID</label>
            <input
              className="input-field"
              placeholder="e.g., alice_dev"
              value={creator}
              onChange={e => setCreator(e.target.value)}
              required
            />
          </div>
          <div>
            <label className="block text-sm font-medium text-slate-300 mb-1">Original IP Content</label>
            <textarea
              className="input-field min-h-[120px] resize-y"
              placeholder="Enter the original content to prove ownership..."
              value={content}
              onChange={e => setContent(e.target.value)}
              required
            />
          </div>
          <button className="btn-primary w-full flex items-center justify-center gap-2">
            <Fingerprint className="w-5 h-5" />
            Generate Proof & Verify
          </button>
        </form>

        {msg && (
          <motion.div initial={{ opacity: 0, y: 10 }} animate={{ opacity: 1, y: 0 }} className={`mt-6 p-4 rounded-xl border flex items-center gap-3 relative z-10 ${isSuccess ? 'bg-green-500/10 border-green-500/20 text-green-400' : 'bg-red-500/10 border-red-500/20 text-red-400'}`}>
            {isSuccess ? <CheckCircle2 className="w-5 h-5 flex-shrink-0" /> : <XCircle className="w-5 h-5 flex-shrink-0" />}
            <p className="text-sm font-medium">{msg}</p>
          </motion.div>
        )}
      </div>
    </motion.div>
  );
}
