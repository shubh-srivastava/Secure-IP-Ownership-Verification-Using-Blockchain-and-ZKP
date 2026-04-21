import { BrowserRouter, Routes, Route, useLocation } from "react-router-dom";
import { AnimatePresence } from "framer-motion";
import Navbar from "./components/Navbar";

import Dashboard from "./pages/Dashboard";
import RegisterUser from "./pages/RegisterUser";
import RegisterIP from "./pages/RegisterIP";
import VerifyIP from "./pages/VerifyIP";
import Blockchain from "./pages/Blockchain";

function AnimatedRoutes() {
  const location = useLocation();
  return (
    <AnimatePresence mode="wait">
      <Routes location={location} key={location.pathname}>
        <Route path="/" element={<Dashboard />} />
        <Route path="/register" element={<RegisterUser />} />
        <Route path="/create" element={<RegisterIP />} />
        <Route path="/verify" element={<VerifyIP />} />
        <Route path="/chain" element={<Blockchain />} />
      </Routes>
    </AnimatePresence>
  );
}

export default function App() {
  return (
    <BrowserRouter>
      <div className="min-h-screen flex flex-col relative overflow-hidden">
        {/* Background ambient blobs */}
        <div className="fixed top-[-20%] left-[-10%] w-[50%] h-[50%] rounded-full bg-indigo-600/20 blur-[120px] pointer-events-none" />
        <div className="fixed bottom-[-20%] right-[-10%] w-[50%] h-[50%] rounded-full bg-purple-600/20 blur-[120px] pointer-events-none" />
        
        <Navbar />
        <main className="flex-1 w-full max-w-6xl mx-auto p-6 relative z-10">
          <AnimatedRoutes />
        </main>
      </div>
    </BrowserRouter>
  );
}
