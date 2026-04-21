import { useState } from "react";
import { motion } from "framer-motion";
import { FilePlus, Link as LinkIcon } from "lucide-react";

const pageVariants = {
  initial: { opacity: 0, y: 20 },
  animate: { opacity: 1, y: 0, transition: { duration: 0.4 } },
  exit: { opacity: 0, y: -20, transition: { duration: 0.3 } }
};

export default function RegisterIP() {
  const [creator, setCreator] = useState("");
  const [content, setContent] = useState("");
  const [msg, setMsg] = useState("");
  const [isSuccess, setIsSuccess] = useState(false);

  async function submit(e) {
    e.preventDefault();
    try {
      const res = await fetch("http://localhost:18080/create", {
        method: "POST",
        headers: { "Content-Type": "application/json" },
        body: JSON.stringify({ creator, content })
      });
      const data = await res.json();

      if (data.success) {
        setIsSuccess(true);
        setMsg("✅ IP registered securely on the blockchain");
        setCreator("");
        setContent("");
      } else {
        setIsSuccess(false);
        setMsg("❌ Duplicate IP detected or user not found");
      }
    } catch {
      setIsSuccess(false);
      setMsg("❌ Server connection failed");
    }
  }

  return (
    <motion.div variants={pageVariants} initial="initial" animate="animate" exit="exit" className="max-w-lg mx-auto mt-12">
      <div className="glass-panel p-8">
        <div className="flex justify-center mb-6">
          <div className="p-4 bg-indigo-500/20 rounded-full">
            <FilePlus className="w-10 h-10 text-indigo-400" />
          </div>
        </div>
        
        <h2 className="text-3xl font-bold text-center mb-2 text-white">Register Asset</h2>
        <p className="text-slate-400 text-center mb-8">Anchor your intellectual property</p>

        <form onSubmit={submit} className="space-y-5">
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
            <label className="block text-sm font-medium text-slate-300 mb-1">IP Content (Secret)</label>
            <textarea
              className="input-field min-h-[120px] resize-y"
              placeholder="Enter your confidential data, code snippet, or text..."
              value={content}
              onChange={e => setContent(e.target.value)}
              required
            />
          </div>
          <button className="btn-primary w-full flex items-center justify-center gap-2">
            <LinkIcon className="w-5 h-5" />
            Mint to Blockchain
          </button>
        </form>

        {msg && (
          <motion.div initial={{ opacity: 0, y: 10 }} animate={{ opacity: 1, y: 0 }} className={`mt-6 p-4 rounded-xl border ${isSuccess ? 'bg-green-500/10 border-green-500/20 text-green-400' : 'bg-red-500/10 border-red-500/20 text-red-400'}`}>
            <p className="text-sm font-medium text-center">{msg}</p>
          </motion.div>
        )}
      </div>
    </motion.div>
  );
}
